
mesh = {
	material = materials.bone,
	
	filename = 'D:/PhdData/Align5000/.deformed/11-11.lua.deformed.vtk',
	--filename = 'D:/PhdData/13-Toruses/torus-4-1-16-16.vtk',
	--filename = 'D:/ScanData/Acetabulum/Hires Cdaver/DICOM/Part-2-4/Part-2-4.ply'
};

fixmesh        = false;
calcgenerators = true;
flattenmesh    = true;

weightType     = "floater"   -- "tutte", "floater", "harmonic" or "wachspress"
spreadType     = "none" -- "none", "yoshizawa" or "awt"

defaultTorus = {
	radius = { 5, 1 },
	resolution = { 64, 64 },
};

cameraPos = {
	{
		orbit    = { 0, 0, 0 }
	},
	{
		orbit    = { 84.0102, 8.56716, -77.1272 },
		rotation = { 43, 0, 44 }
	}
}
	