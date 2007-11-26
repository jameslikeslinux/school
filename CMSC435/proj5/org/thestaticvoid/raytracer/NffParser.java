/*
 * NffParser.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Reads an NFF file as defined at:
 * http://tog.acm.org/resources/SPD/NFF.TXT
 * and returns a Scene.
 */
package org.thestaticvoid.raytracer;

import java.io.*;

public class NffParser {
	private static Scene scene;
	private static BufferedReader reader;
	private static String line;
	
	private static void parseView() throws InvalidNffException, IOException {
		line = reader.readLine();
		String[] tokens = line.split(" ");
		if (tokens[0].equals("from"))
			try {
				scene.setViewFrom(new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3])));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"from %g %g %g\"");
			}
		else
			throw new InvalidNffException("Expected \"from\"");
		
		line = reader.readLine();
		tokens = line.split(" ");
		if (tokens[0].equals("at"))
			try {
				scene.setViewAt(new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3])));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"at %g %g %g\"");
			}
		else
			throw new InvalidNffException("Expected \"at\"");
		
		line = reader.readLine();
		tokens = line.split(" ");
		if (tokens[0].equals("up"))
			try {
				scene.setViewUp(new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3])));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"up %g %g %g\"");
			}
		else
			throw new InvalidNffException("Expected \"up\"");
		
		line = reader.readLine();
		tokens = line.split(" ");
		if (tokens[0].equals("angle"))
			try {
				scene.setViewAngle(Double.parseDouble(tokens[1]));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"angle %g\"");
			}
		else
			throw new InvalidNffException("Expected \"angle\"");
		
		line = reader.readLine();
		tokens = line.split(" ");
		if (tokens[0].equals("hither"))
			try {
				scene.setViewHither(Double.parseDouble(tokens[1]));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"hither %g\"");
			}
		else
			throw new InvalidNffException("Expected \"hither\"");
		
		line = reader.readLine();
		tokens = line.split(" ");
		if (tokens[0].equals("resolution"))
			try {
				scene.setResolution(Integer.parseInt(tokens[1]), Integer.parseInt(tokens[2]));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"resolution %d %d\"");
			}
		else
			throw new InvalidNffException("Expected \"resolution\"");
	}
	
	private static void parseBackground() throws InvalidNffException {
		String[] tokens = line.split(" ");
		try {
			scene.setBackgroundColor(new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3])));
		} catch (Exception e) {
			throw new InvalidNffException("Expected \"b %g %g %g\"");
		}
	}
	
	private static void parseFill() throws InvalidNffException {
		String[] tokens = line.split(" ");
		try {
			scene.setCurrentFill(new Fill(new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3])), Double.parseDouble(tokens[4]), Double.parseDouble(tokens[5]), Double.parseDouble(tokens[6]), Double.parseDouble(tokens[7]), Double.parseDouble(tokens[8])));
		} catch (Exception e) {
			throw new InvalidNffException("Expected \"f %g %g %g %g %g %g %g %g\"");
		}
	}
	
	private static void parsePolygon() throws InvalidNffException {
		String[] tokens = line.split(" ");
		int totalVertices;
		try {
			totalVertices = Integer.parseInt(tokens[1]);
		} catch (Exception e) {
			throw new InvalidNffException("Expected \"p %d\"");
		}
		
		Polygon polygon = new Polygon(scene);
		for (int i = 0; i < totalVertices; i++) {
			try {
				line = reader.readLine();
				tokens = line.split(" ");
				polygon.addVertex(new Vector3(Double.parseDouble(tokens[0]), Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2])));			
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"%g %g %g\"");
			}
		}
		
		scene.addSceneObject(polygon);
	}
	
	private static void parsePolygonalPatch() throws InvalidNffException {
		String[] tokens = line.split(" ");
		int totalVertices;
		try {
			totalVertices = Integer.parseInt(tokens[1]);
		} catch (Exception e) {
			throw new InvalidNffException("Expected \"pp %d\"");
		}
		
		PolygonalPatch polygonalPatch = new PolygonalPatch(scene);
		for (int i = 0; i < totalVertices; i++) {
			try {
				line = reader.readLine();
				tokens = line.split(" ");
				polygonalPatch.addVertex(new Vector3(Double.parseDouble(tokens[0]), Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2])), new Vector3(Double.parseDouble(tokens[3]), Double.parseDouble(tokens[4]), Double.parseDouble(tokens[5])));			
			} catch (Exception e) {
				e.printStackTrace();
				throw new InvalidNffException("Expected \"%g %g %g %g %g %g\"");
			}
		}
		
		scene.addSceneObject(polygonalPatch);
	}
	
	private static void parseSphere() throws InvalidNffException {
		String[] tokens = line.split(" ");
		try {
			scene.addSceneObject(new Sphere(scene, new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3])), Double.parseDouble(tokens[4])));
		} catch (Exception e) {
			throw new InvalidNffException("Expected \"s %g %g %g %g\"");
		}
	}
	
	private static void parseLight() throws InvalidNffException {
		String[] tokens = line.split(" ");
		if (tokens.length == 4)
			try {
				scene.addLight(new Light(new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3]))));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"l %g %g %g\"");
			}
		else if (tokens.length == 7)
			try {
				scene.addLight(new Light(new Vector3(Double.parseDouble(tokens[1]), Double.parseDouble(tokens[2]), Double.parseDouble(tokens[3])), new Vector3(Double.parseDouble(tokens[4]), Double.parseDouble(tokens[5]), Double.parseDouble(tokens[6]))));
			} catch (Exception e) {
				throw new InvalidNffException("Expected \"l %g %g %g %g %g %g\"");
			}
		else
			throw new InvalidNffException("Expected \"l %g %g %g [%g %g %g]\"");
	}
	
	// The only public function, fairly straight-forward.
	public static Scene parse(InputStream input) throws InvalidNffException, IOException {
		scene = new Scene();
		reader = new BufferedReader(new InputStreamReader(input));
		
		while ((line = reader.readLine()) != null) {
			if (line.equals("v"))
				parseView();
			else if (line.startsWith("b "))
				parseBackground();
			else if (line.startsWith("f "))
				parseFill();
			else if (line.startsWith("p "))
				parsePolygon();
			else if (line.startsWith("pp "))
				parsePolygonalPatch();
			else if (line.startsWith("s "))
				parseSphere();
			else if (line.startsWith("l "))
				parseLight();
			else if (line.startsWith("#"));
			else
				throw new InvalidNffException("Unknown line " + line);
		}
		
		return scene;
	}
}