surface tile() {
	normal Nf = faceforward(normalize(N), I);
	point P_world = ntransform("world", P);

	/* Draw background */
	Ci = color(0.5, 0.25, 0.15);
	Ci += (float noise (P_world * 0.25) - 0.5) * 0.2;
	Ci += (float noise (P_world * 10) - 0.5) * 0.3;
	Ci *= ambient() + diffuse(Nf);

	color white_tile = color(0.95, 0.9, 0.75);
	white_tile += (float noise(P_world * 10) - 0.5) * 0.8;
	white_tile += (float noise(P_world * 0.35) - 0.5) * 0.3;
	white_tile = white_tile * (ambient() + 0.5 * diffuse(Nf)) + 1 * 0.5 * specular(Nf, -normalize(I), 0.1);

	color blue_tile = color(0.0, 0.0, 0.1);
	blue_tile += (float noise(P_world * 10) - 0.3) * 2;
	blue_tile = blue_tile * (ambient() + 0.5 * diffuse(Nf)) + 1 * 0.5 * specular(Nf, -normalize(I), 0.1);

	float width = 10;
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

	float x = mod(xcomp(P_world) + 2, width);
	float y = mod(zcomp(P_world) - 3, width);


	/* 
	 * There could be more reuse of code by just rotating the little tile around a
	 * bit but that doesn't work if for some reason the "little" (corner) tile is
	 * larger than the "big" (center) tile.
	 *
	 * In other words, it's just easier to write the code for all like squares.
	 */

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

	Oi = Os;
	Ci *= Oi;
}
