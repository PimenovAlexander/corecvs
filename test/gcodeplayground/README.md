```
Usage:
	help
	compensate [-i|--input filename] [-o|--output filename] [-f|--offset offset] [-t|--touchZ touchZ]
	render [-i|--input filename] [-o|--output filename] [-v|--vinyl]
```

Compensate:
 -   -f - Offset of the knife
 - -t - Height, at which the knife touches the material
    
Render:
   - -v - Render position of the knife (default renders axis position)
   
Examples:
```
./gcodeplayground compensate -i in.gcode -o out.gcode -f 0.5 -t -0.1
./gcodeplayground render -i out.gcode -o out.ply 
```
