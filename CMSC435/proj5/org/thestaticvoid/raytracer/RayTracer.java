/*
 * RayTracer.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Main class to give standard input to the NffParser write the ray traced
 * image to stardard output.
 */
package org.thestaticvoid.raytracer;

import java.io.*;

public class RayTracer {
	public static void main(String[] args) {
		try {
			if (args.length == 2) {
				InputStream in = new FileInputStream(args[0]);
				Scene scene = NffParser.parse(in);
				in.close();
				
				PrintStream out = new PrintStream(args[1]);
				scene.toPpm(out);
				out.close();
			} else {
				Scene scene = NffParser.parse(System.in);
				PrintStream out = new PrintStream(System.out);
				scene.toPpm(out);
			}
		} catch (Exception e) {
			e.printStackTrace();
			System.exit(1);
		}
	}
}
