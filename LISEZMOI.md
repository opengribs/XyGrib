XyGrib est un clone de zyGrib (https://opengribs.org/fr)

Les changements dans la version 1.2.4

- Nouveau modèle régional ajouté : Arome 0.025 deg couverture France et régions environnantes
- La grille du Grib n'est visible que si l'échelle est appropriée, plus d'écran noir !)
- Nombreuses améliorations du code : Merci à Didier
- Corrections de bugs en particulier correction du crash arrivant dans certains cas en éditant un POI ou en ouvrant une MétéoTable

Les changements dans la version 1.2.2

- Nouvelle fonctionnalité - Carte des rafales de vent : affichage soit des valeurs absolues de vitesse de rafale ou les valeurs relatives (écart entre la rafale et la vitesse moyenne du vent). La vitesse de rafale absolue est la valeur par défaut. Cela peut être modifié dans le sous-menu des options présentation graphique du menu Cartes météo.
- La transparence a été ajoutée aux valeurs de couleur les plus basses pour les variables Précipitations, CAPE, Réflectivité, Rafales (écart) et Neige. Cela améliore les affichages de superposition de couleurs.
- La fonctionnalité d'affichage de la couverture des modèles régionaux est améliorée et zoome directement sur la zone ombrée du modèle choisi.
- Les problèmes d'affichage de la couverture nuageuse avec les gribs ICON sont corrigés.
- De toutes petites surfaces peuvent maintenant être sélectionnées. Ce n'est plus considéré comme une erreur ("Bad request")
- Plusieurs autres bugs ont été corrigés et plusieurs améliorations de la stabilité du code ont été incluses.

Les changements dans la version 1.2.0

- Amélioration majeure du serveur Grib et de XyGrib avec l’ajout de modèles régionaux dans plusieurs zones.
- Il s'agit des modèles NAM (CONUS, Amérique centrale et Caraïbes, Pacifique et Hawai), ICON-EU et Arpege-EU.
- ECWMF ajouté aux modèles globaux, bien qu’une version gratuite et limitée du modèle comprenant certaines données de surface et d’altitude à une résolution de 0.5 degré et une résolution temporelle de 24 heures.
- Correction de bugs
- Nombreuses améliorations en termes de stabilité et de vitesse (Merci à Didier)

 Les changements dans la version 1.1.2

- Principalement des corrections de bugs, en particulier en regard de la sauvegarde des paramètres
- Un outil d'installation pour l'environnement OS Mac
- Les menus natifs du système OS Mac mis en place (Merci à Theeko)

Les changements dans la version 1.1.1

- Modification de la compilation en utilisant au lieu de "qmake" "cmake" (Merci à Pavel Kalian)
- Important nettoyage , amélioration et stabilisation du code  (Merci à Didier)
- Compression Jpeg2000 des fichiers grib amliorée pour permettre un fonctionnement sans erreur.  Les données vagues seront compressées de la même manière dans la prochaine version. (Merci à Didier)
- L'application et les données sont séparées dans des répertoires différents pour être plus conforme aux standards linux en particulier. Des améliorations devraient intervenir dans la prochaine version en particulier pour la compilation et l'installation à partir du code source.
- Nouvel outil d'installation pour les binaires. L'outil permet le téléchargement de l'application et des données statiques à partir d'un répertoire en ligne. Il permet également l'intallation optionnelle des cartes à haute résolution. Un outil de maintenance, qui peut être utliisé pour effectuer la mise à jour lorsqu'une nouvelle version est disponible et permet l'installation ou la désinstallation de l'application et/ou des divers composants, y compris les cartes de haute résolution est également installé.

Les changements dans la version 1.1.0

- Téléchargement interactif depuis le nouveau serveur OpenGrib, offrant à la volée le choix entre différents modèles :
- - les modèles numériques de prévision GFS, ICON et Arpège
- - les modèles de vagues WW3, GWAM et EWAM
- Améliorations dans l'utilisation de paramètres alternatifs comme la précipitation totale et la taux de la précipitation (ce n'est pas encore terminé, les données provenant des différents modèles suivent des règles différentes et pas toujours clairement expliquées).
- La vérification au démarrage d'une nouvelle version. Fonctionalité également disponible depuis le menu Aide.
- Nettoyage important du code.
- Ajout de l'hébreu.

Première version de XyGrib; changements depuis la version zyGrib 8.0.1.

- Changement de nom de ZyGrib à XyGrib (nouveau nom du clone)
- Ajout d'un nouveau paramètre météorologique 'Réflectivité composite simulée', ce qui permet de visualiser une convection sévère
- Possibilité de visualiser des fichiers grib modèle ICON du Deutsche Wetter Dienst (DWD)
- Possibilité de visualiser les fichiers grib fourni par l'institut météorologique norvégien
- Possibilité d visualiser les courants de surface produits par l'agence fédérale allemande maritime et hydrographique
- Possibilité de visualiser des données 'vagues' fournies au format grib 2
- Ajout de la gestion des données rafales en surface et à 10m (selon les sources les données rafales ne sont pas fournies à la même hauteur)
- Un nouvel habillage (skin); les deux habillages disponibles sont à choix dans la rubrique option.
