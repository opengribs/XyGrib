# Building XyGrib with direct GRIB download

XyGrib normally fetches GRIB data through the OpenGribs broker
(`grbsrv.opengribs.org`). That broker lost its dataset backend in September 2026 and
downloads through it have not worked since. This branch adds a second path that fetches
straight from upstream providers, so no broker is involved.

## Quick start

```
sudo apt-get install -y cmake g++ qtbase5-dev qttools5-dev qttools5-dev-tools \
                        libproj-dev libnova-dev libopenjp2-7-dev libbz2-dev \
                        zlib1g-dev libpng-dev

cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build -j"$(nproc)"

XYGRIB_LOCAL_DOWNLOAD=1 ./build/src/XyGrib
```

That is all. The configure step needs no extra flags once the packages above are present.

## Which download path runs

| `XYGRIB_LOCAL_DOWNLOAD` | Path taken |
|---|---|
| unset, or `0` / `false` / `no` / `off` | broker, exactly as before |
| `1` / `true` / `yes` / anything else | direct from upstream, no broker |

The variable is read on each download, so a wrapper script can select per run. An
explicit `downloadLocal=true` in the settings file is honoured when the variable is
unset, but the variable wins. **The default when unset is unchanged**: installs on
older machines keep the broker behaviour they were built for.

Settings live in `~/.config/xyGrib.conf`, under `[main]`.

## Models

| Model | Direct download | Notes |
|---|---|---|
| GFS 0.25 / 0.50 / 1.0 | yes | NOAA NOMADS GRIB filter |
| NAM CONUS / CACBN / Pacific | yes | NOAA NOMADS |
| WW3, GWAM | yes | wave fields, same host |
| ICON / ICON-EU | no | refused with a message |
| ECMWF | no | refused with a message |
| Arpege, Arpege-EU, Arome | no | refused with a message |

Unsupported models are refused explicitly rather than half-served. The holdouts cannot
crop a spatial window on the server side, so a single request would mean downloading a
full-grid file — roughly 50 MB for 24 h at 3 h intervals, against tens of kilobytes for
the models above. Serving them properly needs either a broker or a much larger download
budget, and is left as separate work.

## Why this works without a broker

NOMADS crops by bounding box, parameter **and** level server-side:

- `filter_gfs_<res>.pl` for GFS and NAM
- `filter_gfswave.pl`, `filter_gwam.pl` for waves

The provider builds one URL per forecast step; the results are concatenated. A GRIB2
file is a sequence of self-contained messages, so byte concatenation yields a valid
multi-message file — the same shape the reader already handles on load.

The broker existed to do this cropping on a server because, in 2018, providers only
served full-grid files and reassembling them on a boat laptop was impractical. That
constraint no longer applies for the models listed above.

## Verifying a download

The reader, not the downloader, decides whether a file is usable, so check with
XyGrib's own code. A file is good when the reader reports records, dates and a sane
zone, and the expected parameters resolve to varying values:

```
# after a direct download, inspect what the reader actually stored
./dump_codes path/to/your.grb
```

Minimum acceptance for a GFS + WW3 request: `ok=yes`, a non-zero record count, more
than one forecast date, the requested bounding box echoed back, and wind, MSL pressure
and wave fields all resolving with `min != max`.

## Troubleshooting

**Configure fails with `Unknown CMake command "qt5_create_translation"`.**
`Qt5LinguistToolsConfig.cmake` is missing. It ships in `qttools5-dev`, which is a
*different package* from `qttools5-dev-tools`. The `-tools` package provides the
`lrelease` / `lupdate` binaries; the non-`-tools` one provides the CMake config. Both
are needed:

```
sudo apt-get install -y qttools5-dev qttools5-dev-tools
```

If apt claims it is already newest, that is the resolver short-circuiting on the
version-pinned dependencies; `sudo apt-get install --reinstall qttools5-dev` or
installing the `.deb` with `dpkg -i` gets past it.

**Download reports "Upstream returned no GRIB data".**
The requested forecast cycle is not published yet. NOMADS answers a non-200 with an
HTML body in that case, which the client detects by checking for the `GRIB` magic
rather than trusting the status code. Cycles run 00/06/12/18 UTC with roughly a
3.5-5 h publication lag; the client picks the most recent cycle that should be ready.
Retry, or wait for the next cycle.

**Download reports "Local download does not support this model yet".**
Expected for ICON, ECMWF, Arpege and Arome. Unset `XYGRIB_LOCAL_DOWNLOAD` to use the
broker for those, or use a differently supported model.
