# sim-normals
natural looking normal maps from photos based on a photoshop script I create in 2008

The input photos should have diffuse lighting and without significant color contrast, shadows will show as depressions. This will use the value information in an image to create a normal map from a single image. 

This creates a naturaly looking normal map by using pyrimids of gradiants. You can select the size of detail you extract from the image by setting the min_detail and max_detail parameters. A method using tangents and bitagents would be more accurate but this works sufeciantly on organcic subjects (photos). This method may produce ringing on edges in hard surfaces which can be minimize by setting the max_detail parameter but wont eliminate it. 

min_detail gaussian smoothing to remove smaller detials

max_detail high pass to remove larger details, also indirectly controls pyramid levels. 
