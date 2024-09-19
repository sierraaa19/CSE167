#include "hw1.h"
#include "hw1_scenes.h"

using namespace hw1;

Image3 hw_1_1(const std::vector<std::string> &params) {
    // Homework 1.1: render a circle at the specified
    // position, with the specified radius and color.

    // creates canvas image with the given dimensions
    Image3 img(640 /* width */, 480 /* height */);

    // the circle parameters
    Vector2 center = Vector2{img.width / 2 + Real(0.5), img.height / 2 + Real(0.5)};
    Real radius = 100.0;
    Vector3 color = Vector3{1.0, 0.5, 0.5};

    // parse command-line argument, overrides default params
    for (int i = 0; i < (int)params.size(); i++) {
        if (params[i] == "-center") {
            Real x = std::stof(params[++i]);
            Real y = std::stof(params[++i]);
            center = Vector2{x, y};
        } else if (params[i] == "-radius") {
            radius = std::stof(params[++i]);
        } else if (params[i] == "-color") {
            Real r = std::stof(params[++i]);
            Real g = std::stof(params[++i]);
            Real b = std::stof(params[++i]);
            color = Vector3{r, g, b};
        }
    }

    // render the circle on the canvas
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
	    // calculate pixel's center
	    Real centerX = x + 0.5;
            Real centerY = y + 0.5;

	    // calculate distance from pixel center to circle center
	    Real distance = length(center - Vector2{centerX, centerY});
 		
	    // check if pixel is inside circle 
	    if (distance <= radius) {
		img(x, y) = color;
	    } else {
		img(x, y) = Vector3{0.5, 0.5, 0.5};
	    }
        }
    }
    return img;
}

Image3 hw_1_2(const std::vector<std::string> &params) {
    // Homework 1.2: render multiple circles
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    int scene_id = std::stoi(params[0]);
    const CircleScene &scene = hw1_2_scenes[scene_id];

    Image3 img(scene.resolution.x, scene.resolution.y);

    // set the background color
    for (int y = 0; y < img.height; y++) {
	for (int x = 0; x < img.width; x++) {
	    img(x, y) = Vector3{0.5, 0.5, 0.5};
	}
    }

    for (size_t i = 0; i < scene.objects.size(); i++) {
        Circle circle = scene.objects[i];
        Vector2 center = circle.center;
        Real radius = circle.radius;
        Vector3 color = circle.color;

        // calculate the bounding box of circle
        int x_min = std::max(0, static_cast<int>(center.x - radius));
        int x_max = std::min(img.width - 1, static_cast<int>(center.x + radius));
        int y_min = std::max(0, static_cast<int>(center.y - radius));
        int y_max = std::min(img.height - 1, static_cast<int>(center.y + radius));

        // render the circle on the canvas
        for (int y = y_min; y <= y_max; y++) {
            for (int x = x_min; x <= x_max; x++) {
                // calculate distance from pixel center to circle center
                Real distance = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));

                // check if pixel is inside circle
                if (distance <= radius) {
                    img(x, y) = color;
                }
            }
        }
    }

    return img;

/* code before optimization
    for (size_t i = 0; i < scene.objects.size(); i++) {
        Circle circle = scene.objects[i];
	Vector2 center = circle.center;
	Real radius = circle.radius;
	Vector3 color = circle.color;

	// render the circle on the canvas
    	for (int y = 0; y < img.height; y++) {
            for (int x = 0; x < img.width; x++) {
            	// calculate distance from pixel center to circle center
            	Real distance = sqrt((x - center.x) * (x - center.x) + (y - center.y) * (y - center.y));

            	// check if pixel is inside circle
            	if (distance <= radius) {
                   img(x, y) = color;
            	} 
            }
    	}
    }
    return img; */
}

Image3 hw_1_3(const std::vector<std::string> &params) {
    // Homework 1.3: render multiple shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
	    Vector3 pixel_color = scene.background;

	    // iterate over all shapes 
	    for (const Shape &shape : scene.shapes) {
                if (auto *circle = std::get_if<Circle>(&shape)) {
                    // check if pixel is inside circle
                    Real dx = x - circle->center.x;
		    Real dy = y - circle->center.y;
		    Real distance = std::sqrt(dx * dx + dy * dy);
                    if (distance <= circle->radius) {
			pixel_color = circle->color;
		    }
		} else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                    // check if pixel is inside rectangle
                    if (x >= rectangle->p_min.x && x <= rectangle->p_max.x &&
                        y >= rectangle->p_min.y && y <= rectangle->p_max.y) {
                        pixel_color = rectangle->color;
                    }
                } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
		    // check if pixel is inside triangle
                    Vector2 p0 = triangle->p0;
                    Vector2 p1 = triangle->p1;
                    Vector2 p2 = triangle->p2;

                    Vector2 e0 = p1 - p0;
                    Vector2 e1 = p2 - p1;
                    Vector2 e2 = p0 - p2;

                    Vector2 v0 = Vector2(x, y) - p0;
		    Vector2 v1 = Vector2(x, y) - p1;
		    Vector2 v2 = Vector2(x, y) - p2;

                    // calculate dot products
                    Real dot0 = e0.x * v0.y - e0.y * v0.x;
                    Real dot1 = e1.x * v1.y - e1.y * v1.x;
                    Real dot2 = e2.x * v2.y - e2.y * v2.x;

		    if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {
                        pixel_color = triangle->color;
                    }
                }
            }

            img(x, y) = pixel_color;
        }
    }
    return img;
}

Image3 hw_1_4(const std::vector<std::string> &params) {
    // Homework 1.4: render transformed shapes
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
	    // initialize background color
	    Vector3 pixel_color = scene.background;
            
	    // iterate over all shapes
            for (const Shape &shape : scene.shapes) {
		// apply inverse transformation to pixel position
                Matrix3x3 inverse_matrix = inverse(get_transform(shape));
		// transform pixel position
	        Vector3 pixel_position(x, y, 1);
                Vector3 object_space_position = inverse_matrix * pixel_position;
		
                if (auto *circle = std::get_if<Circle>(&shape)) {
      		    // check if transformed pixel is inside circle
		    Real dx = object_space_position.x - circle->center.x;
               	    Real dy = object_space_position.y - circle->center.y;
   	            Real distance = std::sqrt(dx * dx + dy * dy);
		    if (distance <= circle->radius) {
                        pixel_color = circle->color;
                    }
                } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
                    // check if pixel is inside rectangle
                    if (object_space_position.x >= rectangle->p_min.x && object_space_position.x <= rectangle->p_max.x &&
                        object_space_position.y >= rectangle->p_min.y && object_space_position.y <= rectangle->p_max.y) {
                        pixel_color = rectangle->color;
                    }
                } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
                    // check if pixel is inside triangle
                    Vector2 p0 = triangle->p0;
                    Vector2 p1 = triangle->p1;
                    Vector2 p2 = triangle->p2;

                    Vector2 e0 = p1 - p0;
                    Vector2 e1 = p2 - p1;
                    Vector2 e2 = p0 - p2;

                    Vector2 v0 = Vector2(object_space_position.x, object_space_position.y) - p0;
                    Vector2 v1 = Vector2(object_space_position.x, object_space_position.y) - p1;
                    Vector2 v2 = Vector2(object_space_position.x, object_space_position.y) - p2;

                    // calculate dot products
                    Real dot0 = e0.x * v0.y - e0.y * v0.x;
                    Real dot1 = e1.x * v1.y - e1.y * v1.x;
                    Real dot2 = e2.x * v2.y - e2.y * v2.x;

                    if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {
                        pixel_color = triangle->color;
                    }
		}
            }
	    img(x, y) = pixel_color;	
        }
    }
    return img;
}

Image3 hw_1_5(const std::vector<std::string> &params) {
    // Homework 1.5: antialiasing
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 avg_color;
    
    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
	    img(x,y) = scene.background;
	}
    }

    // iterate over all shapes
    for (const Shape &shape : scene.shapes) {
    	if (auto *circle = std::get_if<Circle>(&shape)) {
    	    Matrix3x3 inverse_matrix = inverse(get_transform(shape));
	    for (int y = 0; y < img.height; y++) {
        	for (int x = 0; x < img.width; x++) {
            	    avg_color = Vector3(0.0, 0.0, 0.0);
                    for (int sub_y = 0; sub_y < 4; sub_y++) {
                    	for (int sub_x = 0; sub_x < 4; sub_x++) {
			    Real dx = x + (sub_x + 0.5) / 4;
                            Real dy = y + (sub_y + 0.5) / 4;
                            Vector3 object_space_pixel = inverse_matrix * Vector3{dx, dy, 1.0};
                            Real distance = std::sqrt((object_space_pixel.x - circle->center.x) * (object_space_pixel.x - circle->center.x)
                            	+ (object_space_pixel.y - circle->center.y) * (object_space_pixel.y - circle->center.y));
                            if (distance <= circle->radius) {
                            	avg_color += circle->color;
                            } else {
                                avg_color += img(x, y);
                            }
			}
		    }
		    avg_color /= Real(16);
            	    img(x, y) = avg_color;
		}
	    } 
	} else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
	    Matrix3x3 inverse_matrix = inverse(get_transform(shape));
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    avg_color = Vector3(0.0, 0.0, 0.0);
                    for (int sub_y = 0; sub_y < 4; sub_y++) {
                        for (int sub_x = 0; sub_x < 4; sub_x++) {
			    Vector3 object_space_position = inverse_matrix * Vector3{(x + sub_x / 4.0 + 0.25), (y + sub_y / 4.0 + 0.25), 1.0};
                            if (object_space_position.x >= rectangle->p_min.x && object_space_position.x <= rectangle->p_max.x &&
                                object_space_position.y >= rectangle->p_min.y && object_space_position.y <= rectangle->p_max.y) {
                                avg_color += rectangle->color;
                            } else {
                                avg_color += img(x, y);
                            }
			}
		    }
		    avg_color /= Real(16);
                    img(x, y) = avg_color;
		}
	    }
	} else if (auto *triangle = std::get_if<Triangle>(&shape)) {
	    Matrix3x3 inverse_matrix = inverse(get_transform(shape));
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    avg_color = Vector3(0.0, 0.0, 0.0);
                    for (int sub_y = 0; sub_y < 4; sub_y++) {
                        for (int sub_x = 0; sub_x < 4; sub_x++) {
			    Vector3 object_space_position = inverse_matrix * Vector3{(x + sub_x / 4.0), (y + sub_y / 4.0), 1.0};

                            Vector2 p0 = triangle->p0;
                            Vector2 p1 = triangle->p1;
                            Vector2 p2 = triangle->p2;

                            Vector2 e0 = p1 - p0;
                            Vector2 e1 = p2 - p1;
                            Vector2 e2 = p0 - p2;

                            Vector2 v0 = Vector2(object_space_position.x, object_space_position.y) - p0;
                            Vector2 v1 = Vector2(object_space_position.x, object_space_position.y) - p1;
                            Vector2 v2 = Vector2(object_space_position.x, object_space_position.y) - p2;
                            // calculate dot products
                            Real dot0 = e0.x * v0.y - e0.y * v0.x;
                            Real dot1 = e1.x * v1.y - e1.y * v1.x;
                            Real dot2 = e2.x * v2.y - e2.y * v2.x;
                            if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {
                                avg_color += triangle->color;
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
    }
    return img;
}


Image3 hw_1_6(const std::vector<std::string> &params) {
    // Homework 1.6: alpha blending
    if (params.size() == 0) {
        return Image3(0, 0);
    }

    Scene scene = parse_scene(params[0]);
    std::cout << scene << std::endl;

    Image3 img(scene.resolution.x, scene.resolution.y);
    Vector3 avg_color;

    for (int y = 0; y < img.height; y++) {
        for (int x = 0; x < img.width; x++) {
            img(x,y) = scene.background;
        }
    }

    // iterate over all shapes
    for (const Shape &shape : scene.shapes) {
        if (auto *circle = std::get_if<Circle>(&shape)) {
            Matrix3x3 inverse_matrix = inverse(get_transform(shape));
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    avg_color = Vector3(0.0, 0.0, 0.0);
                    for (int sub_y = 0; sub_y < 4; sub_y++) {
                        for (int sub_x = 0; sub_x < 4; sub_x++) {
                            Real dx = x + (sub_x + 0.5) / 4;
                            Real dy = y + (sub_y + 0.5) / 4;
                            Vector3 object_space_pixel = inverse_matrix * Vector3{dx, dy, 1.0};
                            Real distance = std::sqrt((object_space_pixel.x - circle->center.x) * (object_space_pixel.x - circle->center.x)
                                + (object_space_pixel.y - circle->center.y) * (object_space_pixel.y - circle->center.y));
                            if (distance <= circle->radius) {
                                avg_color += circle->alpha * circle->color + (1 - circle->alpha) * img (x, y);
                            } else {
                                avg_color += img(x, y);
                            }
                        }
                    }
                    avg_color /= Real(16);
                    img(x, y) = avg_color;
                }
            }
        } else if (auto *rectangle = std::get_if<Rectangle>(&shape)) {
            Matrix3x3 inverse_matrix = inverse(get_transform(shape));
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    avg_color = Vector3(0.0, 0.0, 0.0);
                    for (int sub_y = 0; sub_y < 4; sub_y++) {
                        for (int sub_x = 0; sub_x < 4; sub_x++) {
                            Vector3 object_space_position = inverse_matrix * Vector3{(x + sub_x / 4.0 + 0.25), (y + sub_y / 4.0 + 0.25), 1.0};
                            if (object_space_position.x >= rectangle->p_min.x && object_space_position.x <= rectangle->p_max.x &&
                                object_space_position.y >= rectangle->p_min.y && object_space_position.y <= rectangle->p_max.y) {
                                avg_color += rectangle->alpha * rectangle->color + (1 - rectangle->alpha) * img (x, y); 
                            } else {
                                avg_color += img(x, y);
                            }
                        }
                    }
                    avg_color /= Real(16);
                    img(x, y) = avg_color;
                }
            }
        } else if (auto *triangle = std::get_if<Triangle>(&shape)) {
            Matrix3x3 inverse_matrix = inverse(get_transform(shape));
            for (int y = 0; y < img.height; y++) {
                for (int x = 0; x < img.width; x++) {
                    avg_color = Vector3(0.0, 0.0, 0.0);
                    for (int sub_y = 0; sub_y < 4; sub_y++) {
                        for (int sub_x = 0; sub_x < 4; sub_x++) {
                            Vector3 object_space_position = inverse_matrix * Vector3{(x + sub_x / 4.0), (y + sub_y / 4.0), 1.0};

                            Vector2 p0 = triangle->p0;
                            Vector2 p1 = triangle->p1;
                            Vector2 p2 = triangle->p2;

                            Vector2 e0 = p1 - p0;
                            Vector2 e1 = p2 - p1;
                            Vector2 e2 = p0 - p2;

                            Vector2 v0 = Vector2(object_space_position.x, object_space_position.y) - p0;
                            Vector2 v1 = Vector2(object_space_position.x, object_space_position.y) - p1;
                            Vector2 v2 = Vector2(object_space_position.x, object_space_position.y) - p2;
                            // calculate dot products
                            Real dot0 = e0.x * v0.y - e0.y * v0.x;
                            Real dot1 = e1.x * v1.y - e1.y * v1.x;
                            Real dot2 = e2.x * v2.y - e2.y * v2.x;
                            if ((dot0 > 0 && dot1 > 0 && dot2 > 0) || (dot0 < 0 && dot1 < 0 && dot2 < 0)) {
                                avg_color += triangle->alpha * triangle->color + (1 - triangle->alpha) * img (x, y);
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
    }
    return img;  
}
