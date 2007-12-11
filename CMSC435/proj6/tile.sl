/*
 * tile.sl
 * James Lee <jlee23@umbc.edu>
 * 
 * This procedurally shades a surface to look as if it's been tiled by
 * floor-tile-found-from-the-god-of-google.jpg
 */

surface tile(float width = 10) {
	normal Nf = faceforward(normalize(N), I);

	/* Translate the given point to be relative to the shader rather than the view */
	point P_world = transform("shader", P);

	/* Draw background */
	Ci = color(0.5, 0.25, 0.15);				/* A dark reddish-brown */
	Ci += (float noise (P_world * 0.25) - 0.5) * 0.2;	/* Add subtle darker and lighter regions */
	Ci += (float noise (P_world * 10) - 0.5) * 0.3;		/* Make it appear coarse */
	Ci *= ambient() + diffuse(Nf);				/* Light like the matte shader */

	/* Define white tile color */
	color white_tile = color(0.95, 0.9, 0.75);			/* A bright creamy-white */
	white_tile += (float noise(P_world * 10) - 0.5) * 0.8;		/* Make it spotty */
	white_tile += (float noise(P_world * 0.35) - 0.5) * 0.3;	/* Give it a dirty appearance */
	white_tile = white_tile * (ambient() + 0.5 * diffuse(Nf)) + 1 * 0.5 * specular(Nf, -normalize(I), 0.1);	/* Light it like the plastic shader */

	/* Define blue tile color */
	color blue_tile = color(0.0, 0.0, 0.1);			/* A very dark blue */
	blue_tile += (float noise(P_world * 10) - 0.3) * 2;	/* Add lots of very bright spots to it */
	blue_tile = blue_tile * (ambient() + 0.5 * diffuse(Nf)) + 1 * 0.5 * specular(Nf, -normalize(I), 0.1);	/* Light it like the plastic shader */

	/* 
	 * The relative point in the tile will be checked against these values 
	 * to determine which color to shade a particular region
	 */
	float grout_width = width * 0.01;
	float small_tile_begin = grout_width;
	float big_tile_end = width / 2 - grout_width / 2;
	float small_tile_end = (big_tile_end - small_tile_begin) / 2.75 - grout_width / 2;
	float big_tile_begin = small_tile_end + grout_width;
	float size_of_missing_chunk = (small_tile_end - small_tile_begin) / 2.75;

	float edge_blue_tile_center = small_tile_end + grout_width / 2;
	float edge_blue_tile_end = edge_blue_tile_center + size_of_missing_chunk - grout_width / 2;
	float edge_blue_tile_begin = edge_blue_tile_center - size_of_missing_chunk + grout_width / 2;

	float center_blue_tile_end = width / 2;
	float center_blue_tile_begin = center_blue_tile_end - size_of_missing_chunk + grout_width / 2;

	float blue_tile_width = center_blue_tile_end - center_blue_tile_begin;

	/* Divide the surface into many tiles and get how far we are into this particular one */
	float x = mod(xcomp(P_world), width);
	float y = mod(zcomp(P_world) + width / 2, width);


	/* 
	 * There could be more reuse of code by just rotating the little tile around a
	 * bit but that doesn't work if for some reason the "little" (corner) tile is
	 * larger than the "big" (center) tile.
	 *
	 * In other words, it's just easier to write the code for all like squares.
	 */

	/* Only write the code for one quarter of the tile and repeat it four times, tranlating x, y around */
	float i;
	for (i = 0; i < 4; i++) {
		/* Draw (0, 0) square */
		if (x >= small_tile_begin && x <= small_tile_end &&
		    y >= small_tile_begin && y <= small_tile_end) {
			float end_without_chunk = small_tile_end - size_of_missing_chunk;

			if (x <= end_without_chunk || y <= end_without_chunk || x - end_without_chunk + y - end_without_chunk <= size_of_missing_chunk)
				Ci = white_tile;
		}

		/* Draw (0, 0 blue tile */
		if ((x >= edge_blue_tile_begin && x <= edge_blue_tile_center &&
		     y >= edge_blue_tile_begin && y <= edge_blue_tile_center &&
		     edge_blue_tile_center - x + edge_blue_tile_center - y <= blue_tile_width) ||
		    (x >= edge_blue_tile_center && x <= edge_blue_tile_end &&
		     y >= edge_blue_tile_begin && y <= edge_blue_tile_center &&
		     x - edge_blue_tile_center + edge_blue_tile_center - y <= blue_tile_width) ||
		    (x >= edge_blue_tile_center && x <= edge_blue_tile_end &&
		     y >= edge_blue_tile_center && y <= edge_blue_tile_end &&
		     x - edge_blue_tile_center + y - edge_blue_tile_center <= blue_tile_width) || 
		    (x >= edge_blue_tile_begin && x <= edge_blue_tile_center &&
		     y >= edge_blue_tile_center && y <= edge_blue_tile_end &&
		     edge_blue_tile_center - x + y - edge_blue_tile_center <= blue_tile_width))
			Ci = blue_tile;

		/* Draw (0, 1) and (1, 0) squares */
		float j;
		for (j = 0; j < 2; j++) {
			if (x >= small_tile_begin && x <= small_tile_end &&
			    y >= big_tile_begin && y <= big_tile_end) {
				float x_end_without_chunk = small_tile_end - size_of_missing_chunk;
				float low_y_end_without_chunk = big_tile_begin + size_of_missing_chunk;
				float high_y_end_without_chunk = big_tile_end - size_of_missing_chunk;
		
				if (x <= x_end_without_chunk || ((y >= low_y_end_without_chunk || x - x_end_without_chunk + low_y_end_without_chunk - y <= size_of_missing_chunk) && 
								 (y <= high_y_end_without_chunk || x - x_end_without_chunk + y - high_y_end_without_chunk <= size_of_missing_chunk)))
					Ci = white_tile;
			}

			/* Draw (0, 1) and (1, 0) blue tiles */
			if ((x >= edge_blue_tile_begin && x <= edge_blue_tile_center &&
			     y >= center_blue_tile_begin && y <= center_blue_tile_end &&
			     edge_blue_tile_center - x + center_blue_tile_end - y <= blue_tile_width) ||
			    (x >= edge_blue_tile_center && x <= edge_blue_tile_end &&
			     y >= center_blue_tile_begin && y <= center_blue_tile_end &&
			     x - edge_blue_tile_center + center_blue_tile_end - y <= blue_tile_width))
				Ci = blue_tile;

			float temp = x;
			x = y;
			y = temp;
		}

		/* Draw (1, 1) square */
		if (x >= big_tile_begin && x <= big_tile_end &&
		    y >= big_tile_begin && y <= big_tile_end) {
			float low_end_without_chunk = big_tile_begin + size_of_missing_chunk;
			float high_end_without_chunk = big_tile_end - size_of_missing_chunk;

			if ((x >= low_end_without_chunk || y >= low_end_without_chunk || low_end_without_chunk - x + low_end_without_chunk - y <= size_of_missing_chunk) &&
			    (x >= low_end_without_chunk || y <= high_end_without_chunk || low_end_without_chunk - x + y - high_end_without_chunk <= size_of_missing_chunk) &&
			    (x <= high_end_without_chunk || y >= low_end_without_chunk || x - high_end_without_chunk + low_end_without_chunk - y <= size_of_missing_chunk) &&
			    (x <= high_end_without_chunk || y <= high_end_without_chunk || x - high_end_without_chunk + y - high_end_without_chunk <= size_of_missing_chunk))
				Ci = white_tile;
		}

		/* Draw (1, 1) blue tile */
		if (x >= center_blue_tile_begin && x <= center_blue_tile_end &&
		    y >= center_blue_tile_begin && y <= center_blue_tile_end &&
		    center_blue_tile_end - x + center_blue_tile_end - y <= blue_tile_width)
			Ci = blue_tile;

		if (mod(i, 2) == 0)
			x = width - x;
		else
			y = width - y;
	}

	/* Set the opacity */
	Oi = Os;
	Ci *= Oi;
}
