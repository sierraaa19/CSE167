#include "hw2.h"
#include "hw2_scenes.h"

using namespace hw2;

Image3 hw_2_1(const std::vector<std::string> &params) {
    // Homework 2.1: render a single 3D triangle

    Image3 img(640 /* width */, 480 /* height */);

    Vector3 p0{0, 0, -1};
    Vector3 p1{1, 0, -1};
    Vector3 p2{0, 1, -1};
    Real s = 1; // scaling factor of the view frustrum
    Real a = (Real)img.width/(Real)img.height; // aspect ratio
    Vector3 color = Vector3{1.0, 0.5, 0.5};
    Real z_near = 1e-6; // distance of the near clipping plane
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-p0") {
            p0.x = std::stof(params[++i]);
            p0.y = std::stof(params[++i]);
            p0.z = std::stof(params[++i]);
        } else if (params[i] == "-p1") {
            p1.x = std::stof(params[++i]);
            p1.y = std::stof(params[++i]);
            p1.z = std::stof(params[++i]);
        } else if (params[i] == "-p2") {
            p2.x = std::stof(params[++i]);
            p2.y = std::stof(params[++i]);
            p2.z = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        }
    }

    // perspective projection
    Vector2 p0_proj, p1_proj, p2_proj;
    p0_proj.x = -p0.x / p0.z;
    p0_proj.y = -p0.y / p0.z;
    p1_proj.x = -p1.x / p1.z;
    p1_proj.y = -p1.y / p1.z;
    p2_proj.x = -p2.x / p2.z;
    p2_proj.y = -p2.y / p2.z;

    Vector3 avg_color;

    // set background color
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5}; 
        }
    }

    // check if point is in front of the near clipping plane
    if (p0.z <= z_near && p1.z <= z_near && p2.z <= z_near) {
	// convert the point from the projected camera space to image space
	p0_proj.x = (img.width * (p0_proj.x + (s * a))) / (2.0 * (s * a));
	p0_proj.y = (-img.height * (p0_proj.y - s)) / (2.0 * s);
	p1_proj.x = (img.width * (p1_proj.x + (s * a))) / (2.0 * (s * a));
        p1_proj.y = (-img.height * (p1_proj.y - s)) / (2.0 * s);
	p2_proj.x = (img.width * (p2_proj.x + (s * a))) / (2.0 * (s * a));
        p2_proj.y = (-img.height * (p2_proj.y - s)) / (2.0 * s);
	
	for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
        	avg_color = Vector3(0.0, 0.0, 0.0);
                for (int sub_y = 0; sub_y < 4; sub_y++) {
                    for (int sub_x = 0; sub_x < 4; sub_x++) {
			Real dx = x + (sub_x + 0.5) / 4;
                        Real dy = y + (sub_y + 0.5) / 4;
                        
			// calculate edges of triangle    
                        Vector2 e0 = p1_proj - p0_proj;
                        Vector2 e1 = p2_proj - p1_proj;
                        Vector2 e2 = p0_proj - p2_proj;
			
			// calculate normal vector
			Vector2 n0(e0.y, -e0.x);
			Vector2 n1(e1.y, -e1.x);
			Vector2 n2(e2.y, -e2.x);

                        // calculate dot products
                        Real dot0 = dot((Vector2(dx, dy) - p0_proj), n0);
                        Real dot1 = dot((Vector2(dx, dy) - p1_proj), n1);
                        Real dot2 = dot((Vector2(dx, dy) - p2_proj), n2);
                        
			if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {
                            avg_color += color;
                        } else {
                            avg_color += img(x, y);
                        }
		    }
		}
		avg_color /= Real(16);
                img(x, y) = avg_color;
	    }
	}
    }

    return img;
}

Image3 hw_2_2(const std::vector<std::string> &params) {
    // Homework 2.2: render a triangle mesh

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real a = (Real)img.width/(Real)img.height; // aspect ratio
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    // set background color
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5}; 
        }
    }

    Vector3 avg_color;

    for (int y = 0; y < img.height; y++) {
    	for (int x = 0; x < img.width; x++) {
            avg_color = Vector3(0.0, 0.0, 0.0);
            for (int sub_y = 0; sub_y < 4; sub_y++) {
        	for (int sub_x = 0; sub_x < 4; sub_x++) {
		    Real dx = x + (sub_x + 0.5) / 4;
                    Real dy = y + (sub_y + 0.5) / 4;
		    // projected point
                    Vector3 pointP = Vector3(dx, dy, -1.0);
		    // initialize z-buffer
		    Real z_min = std::numeric_limits<Real>::max();
		    
		    Vector3 pixel_color = Vector3(0.5, 0.5, 0.5);

		    // loop through the triangles in the mesh
		    for (size_t i = 0; i < mesh.faces.size(); i++) {
        		// access verticies
			Vector3i face = mesh.faces[i];
        		Vector3 p0 = mesh.vertices[face[0]];
        		Vector3 p1 = mesh.vertices[face[1]];
        		Vector3 p2 = mesh.vertices[face[2]];
        		Vector3 triangleColor = mesh.face_colors[i];

			// check if point is in front of the near clipping plane
                        if (p0.z <= z_near && p1.z <= z_near && p2.z <= z_near) {

			    // perspective projection
        		    Vector2 p0_proj, p1_proj, p2_proj;
        		    p0_proj.x = -p0.x / p0.z;
        		    p0_proj.y = -p0.y / p0.z;
        		    p1_proj.x = -p1.x / p1.z;
        		    p1_proj.y = -p1.y / p1.z;
        		    p2_proj.x = -p2.x / p2.z;
        		    p2_proj.y = -p2.y / p2.z;	

			    // convert the point from the projected camera space to image space
            		    p0_proj.x = (img.width * (p0_proj.x + (s * a))) / (2.0 * (s * a));
            		    p0_proj.y = (-img.height * (p0_proj.y - s)) / (2.0 * s);
            		    p1_proj.x = (img.width * (p1_proj.x + (s * a))) / (2.0 * (s * a));
            		    p1_proj.y = (-img.height * (p1_proj.y - s)) / (2.0 * s);
            		    p2_proj.x = (img.width * (p2_proj.x + (s * a))) / (2.0 * (s * a));
            		    p2_proj.y = (-img.height * (p2_proj.y - s)) / (2.0 * s);

			    // calculate edges of triangle
                            Vector2 e0 = p1_proj - p0_proj;
                            Vector2 e1 = p2_proj - p1_proj;
                            Vector2 e2 = p0_proj - p2_proj;

                            // calculate normal vector
                            Vector2 n0(e0.y, -e0.x);
                            Vector2 n1(e1.y, -e1.x);
                            Vector2 n2(e2.y, -e2.x);

                            // calculate dot products
                            Real dot0 = dot((Vector2(dx, dy) - p0_proj), n0);
                            Real dot1 = dot((Vector2(dx, dy) - p1_proj), n1);
                            Real dot2 = dot((Vector2(dx, dy) - p2_proj), n2);			

                            if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {
				
			    	// 3d barycentric coordinates
                        	Vector3 p0_proj3d = Vector3(p0_proj.x, p0_proj.y, -1.0);
                                Vector3 p1_proj3d = Vector3(p1_proj.x, p1_proj.y, -1.0);
                                Vector3 p2_proj3d = Vector3(p2_proj.x, p2_proj.y, -1.0);

                        	// calculate edges of 3d triangle
                        	Vector3 e3d0 = p1_proj3d - p0_proj3d;
                        	Vector3 e3d1 = p2_proj3d - p1_proj3d;
                        	Real area = (length(cross(e3d0, e3d1)))/2;

			    	// barycentric coordinates
                            	// b0'
                            	Vector3 p1p = p1_proj3d - pointP;
                            	Vector3 p2p1 = p2_proj3d - p1_proj3d;
                            	Real area0 = (length(cross(p1p, p2p1)))/2;
                            	Real b0_proj = area0/area;
                            	// b1'
                            	Vector3 p2p = p2_proj3d - pointP;
                            	Vector3 p0p2 = p0_proj3d - p2_proj3d;
                            	Real area1 = (length(cross(p2p, p0p2)))/2;
                            	Real b1_proj = area1/area;
                            	// b2'
                            	Vector3 p0p = p0_proj3d - pointP;
                            	Vector3 p1p0 = p1_proj3d - p0_proj3d;
                            	Real area2 = (length(cross(p0p, p1p0))/2);
                            	Real b2_proj = area2/area;

			    	// (14)
                            	Real b0 = (b0_proj/p0.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));
                            	Real b1 = (b1_proj/p1.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));
                            	Real b2 = (b2_proj/p2.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));

                            	// (15)
			    	Real depth = 1 / ((b0 * (1 / p0.z)) + (b1 * (1 / p1.z)) + (b2 * (1 / p2.z)));
                            	Real absDepth = abs(depth);

				if (absDepth < abs(z_min)) {
				    z_min = absDepth;
				    // update pixel color
                                    pixel_color = triangleColor;
                                }
                            }
			}
		    }
		    avg_color += pixel_color;
		}
	    }
	    avg_color /= Real(16);
            img(x, y) = avg_color;
	}
    }
    return img;
}

Image3 hw_2_3(const std::vector<std::string> &params) {
    // Homework 2.3: render a triangle mesh with vertex colors

    Image3 img(640 /* width */, 480 /* height */);

    Real s = 1; // scaling factor of the view frustrum
    Real a = (Real)img.width/(Real)img.height; // aspect ratio
    Real z_near = 1e-6; // distance of the near clipping plane
    int scene_id = 0;
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-s") {
            s = std::stof(params[++i]);
        } else if (params[i] == "-znear") {
            z_near = std::stof(params[++i]);
        } else if (params[i] == "-scene_id") {
            scene_id = std::stoi(params[++i]);
        }
    }

    TriangleMesh mesh = meshes[scene_id];

    // set background color
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = Vector3{0.5, 0.5, 0.5};
        }
    }

    Vector3 avg_color;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            avg_color = Vector3(0.0, 0.0, 0.0);
            for (int sub_y = 0; sub_y < 4; sub_y++) {
                for (int sub_x = 0; sub_x < 4; sub_x++) {
                    Real dx = x + (sub_x + 0.5) / 4;
                    Real dy = y + (sub_y + 0.5) / 4;
                    // projected point
                    Vector3 pointP = Vector3(dx, dy, -1.0);
                    // initialize z-buffer
                    Real z_min = std::numeric_limits<Real>::max();

                    Vector3 pixel_color = Vector3(0.5, 0.5, 0.5);

                    // loop through the triangles in the mesh
                    for (size_t i = 0; i < mesh.faces.size(); i++) {
                        // access verticies
                        Vector3i face = mesh.faces[i];
                        Vector3 p0 = mesh.vertices[face[0]];
                        Vector3 p1 = mesh.vertices[face[1]];
                        Vector3 p2 = mesh.vertices[face[2]];

			// access vertex colors
			Vector3 c0 = mesh.vertex_colors[face[0]];
                        Vector3 c1 = mesh.vertex_colors[face[1]];
                        Vector3 c2 = mesh.vertex_colors[face[2]];

                        // check if point is in front of the near clipping plane
                        if (p0.z <= z_near && p1.z <= z_near && p2.z <= z_near) {

                            // perspective projection
                            Vector2 p0_proj, p1_proj, p2_proj;
                            p0_proj.x = -p0.x / p0.z;
                            p0_proj.y = -p0.y / p0.z;
                            p1_proj.x = -p1.x / p1.z;
                            p1_proj.y = -p1.y / p1.z;
                            p2_proj.x = -p2.x / p2.z;
                            p2_proj.y = -p2.y / p2.z;

                            // convert the point from the projected camera space to image space
                            p0_proj.x = (img.width * (p0_proj.x + (s * a))) / (2.0 * (s * a));
                            p0_proj.y = (-img.height * (p0_proj.y - s)) / (2.0 * s);
                            p1_proj.x = (img.width * (p1_proj.x + (s * a))) / (2.0 * (s * a));
                            p1_proj.y = (-img.height * (p1_proj.y - s)) / (2.0 * s);
                            p2_proj.x = (img.width * (p2_proj.x + (s * a))) / (2.0 * (s * a));
                            p2_proj.y = (-img.height * (p2_proj.y - s)) / (2.0 * s);

                            // calculate edges of triangle
                            Vector2 e0 = p1_proj - p0_proj;
                            Vector2 e1 = p2_proj - p1_proj;
                            Vector2 e2 = p0_proj - p2_proj;

                            // calculate normal vector
                            Vector2 n0(e0.y, -e0.x);
                            Vector2 n1(e1.y, -e1.x);
                            Vector2 n2(e2.y, -e2.x);

                            // calculate dot products
                            Real dot0 = dot((Vector2(dx, dy) - p0_proj), n0);
                            Real dot1 = dot((Vector2(dx, dy) - p1_proj), n1);
                            Real dot2 = dot((Vector2(dx, dy) - p2_proj), n2);

                            if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {

                                // 3d barycentric coordinates
                                Vector3 p0_proj3d = Vector3(p0_proj.x, p0_proj.y, -1.0);
                                Vector3 p1_proj3d = Vector3(p1_proj.x, p1_proj.y, -1.0);
                                Vector3 p2_proj3d = Vector3(p2_proj.x, p2_proj.y, -1.0);

                                // calculate edges of 3d triangle
                                Vector3 e3d0 = p1_proj3d - p0_proj3d;
                                Vector3 e3d1 = p2_proj3d - p1_proj3d;
                                Real area = (length(cross(e3d0, e3d1)))/2;

                                // barycentric coordinates
                                // b0'
                                Vector3 p1p = p1_proj3d - pointP;
                                Vector3 p2p1 = p2_proj3d - p1_proj3d;
                                Real area0 = (length(cross(p1p, p2p1)))/2;
                                Real b0_proj = area0/area;
                                // b1'
                                Vector3 p2p = p2_proj3d - pointP;
                                Vector3 p0p2 = p0_proj3d - p2_proj3d;
                                Real area1 = (length(cross(p2p, p0p2)))/2;
                                Real b1_proj = area1/area;
                                // b2'
                                Vector3 p0p = p0_proj3d - pointP;
                                Vector3 p1p0 = p1_proj3d - p0_proj3d;
                                Real area2 = (length(cross(p0p, p1p0))/2);
                                Real b2_proj = area2/area;

                                // (14)
                                Real b0 = (b0_proj/p0.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));
                                Real b1 = (b1_proj/p1.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));
                                Real b2 = (b2_proj/p2.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));

                                // (15)
                                Real depth = 1 / ((b0 * (1 / p0.z)) + (b1 * (1 / p1.z)) + (b2 * (1 / p2.z)));
                                Real absDepth = abs(depth);

                                if (absDepth < abs(z_min)) {
                                    z_min = absDepth;
				    // interpolate vertex colors
				    Vector3 c = b0 * c0 + b1 * c1 + b2 * c2;
                                    // update pixel color
                                    pixel_color = c;
                                }
                            }
                        }
                    }
                    avg_color += pixel_color;
                }
            }
            avg_color /= Real(16);
            img(x, y) = avg_color;
        }
    }
    return img;
}

Image3 hw_2_4(const std::vector<std::string> &params) {
    // Homework 2.4: render a scene with transformation
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.camera.resolution.x,
               scene.camera.resolution.y);

    Real s = 1; // scaling factor of the view frustrum
    Real a = (Real)img.width/(Real)img.height; // aspect ratio
    Real z_near = 1e-6; // distance of the near clipping plane

    // set background color
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x, y) = scene.background;
        }
    }

    Vector3 avg_color;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            avg_color = Vector3(0.0, 0.0, 0.0);
            for (int sub_y = 0; sub_y < 4; sub_y++) {
                for (int sub_x = 0; sub_x < 4; sub_x++) {
                    // transform pixel position
		    // Matrix4x4 inverse_matrix = inverse(cam_to_world);
		    Real dx = x + (sub_x + 0.5) / 4;
                    Real dy = y + (sub_y + 0.5) / 4;
                    // projected point
                    Vector3 pointP = Vector3(dx, dy, -1.0);
                    // initialize z-buffer
                    Real z_min = std::numeric_limits<Real>::max();

                    Vector3 pixel_color = Vector3(0.5, 0.5, 0.5);

                    // loop through the triangles in the mesh
                    for (size_t i = 0; i < mesh.faces.size(); i++) {
                        // access verticies
                        Vector3i face = mesh.faces[i];
                        Vector3 p0 = mesh.vertices[face[0]];
                        Vector3 p1 = mesh.vertices[face[1]];
                        Vector3 p2 = mesh.vertices[face[2]];

                        // access vertex colors
                        Vector3 c0 = mesh.vertex_colors[face[0]];
                        Vector3 c1 = mesh.vertex_colors[face[1]];
                        Vector3 c2 = mesh.vertex_colors[face[2]];

                        // check if point is in front of the near clipping plane
                        if (p0.z <= z_near && p1.z <= z_near && p2.z <= z_near) {

                            // perspective projection
                            Vector2 p0_proj, p1_proj, p2_proj;
                            p0_proj.x = -p0.x / p0.z;
                            p0_proj.y = -p0.y / p0.z;
                            p1_proj.x = -p1.x / p1.z;
                            p1_proj.y = -p1.y / p1.z;
                            p2_proj.x = -p2.x / p2.z;
                            p2_proj.y = -p2.y / p2.z;

                            // convert the point from the projected camera space to image space
                            p0_proj.x = (img.width * (p0_proj.x + (s * a))) / (2.0 * (s * a));
                            p0_proj.y = (-img.height * (p0_proj.y - s)) / (2.0 * s);
                            p1_proj.x = (img.width * (p1_proj.x + (s * a))) / (2.0 * (s * a));
                            p1_proj.y = (-img.height * (p1_proj.y - s)) / (2.0 * s);
                            p2_proj.x = (img.width * (p2_proj.x + (s * a))) / (2.0 * (s * a));
                            p2_proj.y = (-img.height * (p2_proj.y - s)) / (2.0 * s);

                            // calculate edges of triangle
                            Vector2 e0 = p1_proj - p0_proj;
                            Vector2 e1 = p2_proj - p1_proj;
                            Vector2 e2 = p0_proj - p2_proj;

                            // calculate normal vector
                            Vector2 n0(e0.y, -e0.x);
                            Vector2 n1(e1.y, -e1.x);
                            Vector2 n2(e2.y, -e2.x);

                            // calculate dot products
                            Real dot0 = dot((Vector2(dx, dy) - p0_proj), n0);
                            Real dot1 = dot((Vector2(dx, dy) - p1_proj), n1);
                            Real dot2 = dot((Vector2(dx, dy) - p2_proj), n2);

                            if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {

                                // 3d barycentric coordinates
                                Vector3 p0_proj3d = Vector3(p0_proj.x, p0_proj.y, -1.0);
                                Vector3 p1_proj3d = Vector3(p1_proj.x, p1_proj.y, -1.0);
                                Vector3 p2_proj3d = Vector3(p2_proj.x, p2_proj.y, -1.0);

                                // calculate edges of 3d triangle
                                Vector3 e3d0 = p1_proj3d - p0_proj3d;
                                Vector3 e3d1 = p2_proj3d - p1_proj3d;
                                Real area = (length(cross(e3d0, e3d1)))/2;

                                // barycentric coordinates
                                // b0'
                                Vector3 p1p = p1_proj3d - pointP;
                                Vector3 p2p1 = p2_proj3d - p1_proj3d;
                                Real area0 = (length(cross(p1p, p2p1)))/2;
                                Real b0_proj = area0/area;
                                // b1'
                                Vector3 p2p = p2_proj3d - pointP;
                                Vector3 p0p2 = p0_proj3d - p2_proj3d;
                                Real area1 = (length(cross(p2p, p0p2)))/2;
                                Real b1_proj = area1/area;
                                // b2'
                                Vector3 p0p = p0_proj3d - pointP;
                                Vector3 p1p0 = p1_proj3d - p0_proj3d;
                                Real area2 = (length(cross(p0p, p1p0))/2);
                                Real b2_proj = area2/area;

                                // (14)
                                Real b0 = (b0_proj/p0.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));
                                Real b1 = (b1_proj/p1.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));
                                Real b2 = (b2_proj/p2.z)/((b0_proj/p0.z) + (b1_proj/p1.z) + (b2_proj/p2.z));

                                // (15)
                                Real depth = 1 / ((b0 * (1 / p0.z)) + (b1 * (1 / p1.z)) + (b2 * (1 / p2.z)));
                                Real absDepth = abs(depth);

                                if (absDepth < abs(z_min)) {
                                    z_min = absDepth;
                                    // interpolate vertex colors
                                    Vector3 c = b0 * c0 + b1 * c1 + b2 * c2;
                                    // update pixel color
                                    pixel_color = c;
                                }
                            }
                        }
                    }
                    avg_color += pixel_color;
                }
            }
            avg_color /= Real(16);
            img(x, y) = avg_color;
        }
    }
    return img;
}

