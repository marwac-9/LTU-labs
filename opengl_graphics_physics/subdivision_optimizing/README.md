# Subdivision - optimized

Second approach at geometry subdivision. I utilized things like SSE for more complex calculations and memory pool allocator instead of "new".

Run the application from command prompt using parameters two parameters: numberOfSubdivisions nameOfMeshToSubdivide.

Application will automatically generate the log.txt file containing information for each subdivision.

Result of cube subdivision 8 times:  
5.62732s - Not optimized  
0.69004s - Optimized
