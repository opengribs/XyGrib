function Controller()
{
}


Controller.prototype.installationFinished = function()
{
    controller.createOperations();
	
	controller.addOperation("CreateShortcut", "@TargetDir@/xygrib_maintenancetool.exe", "@StartMenuDir@/XyGribMaintenanceTool.lnk",
            "workingDirectory=@TargetDir@", "iconPath=@TargetDir@/XyGrib.exe", "IconID=1",
             "description=Add, update or remove XyGrib components");
}
