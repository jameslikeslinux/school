/*
 * Scene.java
 * James Lee <jlee23@umbc.edu>
 * 
 * Contains information about the scene as parsed from the NFF file and
 * contains code to ray-trace and dump to a PPM file.
 */
package org.thestaticvoid.raytracer;

import java.io.*;
import java.util.*;
import java.text.*;

public class Scene {
	private Vector3 viewFrom;
	private Vector3 viewAt;
	private Vector3 viewUp;
	private double viewAngle;
	private double viewHither;
	private int resolutionX, resolutionY;
	private Vector3 backgroundColor;
	private Vector3 fillColor;
	private List<SceneObject> sceneObjects;
	private Vector3[][] pixels;
	private int linesCompleted;
	
	public Scene() {
		sceneObjects = new ArrayList<SceneObject>();
	}
	
	public void setViewFrom(Vector3 viewFrom) {
		this.viewFrom = viewFrom;
	}
	
	public void setViewAt(Vector3 viewAt) {
		this.viewAt = viewAt;
	}
	
	public void setViewUp(Vector3 viewUp) {
		this.viewUp = viewUp;
	}
	
	public void setViewAngle(double viewAngle) {
		this.viewAngle = viewAngle;
	}
	
	public void setViewHither(double viewHither) {
		this.viewHither = viewHither;
	}
	
	public void setResolution(int resolutionX, int resolutionY) {
		this.resolutionX = resolutionX;
		this.resolutionY = resolutionY;
		pixels = new Vector3[resolutionX][resolutionY];
	}
	
	public void setBackgroundColor(Vector3 backgroundColor) {
		this.backgroundColor = backgroundColor;
	}
	
	public Vector3 getFillColor() {
		return fillColor;
	}
	
	public void setFillColor(Vector3 fillColor) {
		this.fillColor = fillColor;
	}
	
	public void addSceneObject(SceneObject sceneObject) {
		sceneObjects.add(sceneObject);
	}
	
	private void rayTrace() {
		linesCompleted = 0;
		
		// This class will be instantiated and run for each available processor.
		class RayTraceThread extends Thread {
			private int startY, endY;
			private double nearestT;
			private SceneObject nearestObject;
			
			public RayTraceThread(int startY, int endY) {
				this.startY = startY;
				this.endY = endY;
				nearestT = Double.POSITIVE_INFINITY;
			}
			
			public void run() {
				Vector3 cameraToCenter = viewAt.subtract(viewFrom);
				Vector3 viewRight = cameraToCenter.crossProduct(viewUp).normalize();
				Vector3 viewUpReal = viewRight.crossProduct(cameraToCenter).normalize();
				double cameraToCenterLength = cameraToCenter.length();
				
				// For each pixel assigned to this thread
				for (int y = startY; y < endY; y++) {
					for (int x = 0; x < resolutionX; x++) {
						// Get how far away this pixel is from the center in pixels
						int xDistFromCenter = (x + resolutionX / 2) - resolutionX;
						int yDistFromCenter = (y + resolutionY / 2) - resolutionY;

						// Get how far away this pixel is from the center in world units
						double dx = Math.tan(Math.toRadians((double) xDistFromCenter / resolutionX * viewAngle)) * cameraToCenterLength;
						double dy = Math.tan(Math.toRadians((double) yDistFromCenter / resolutionY * viewAngle)) * cameraToCenterLength;
						
						// Rotate the cameras view vector to the point
						Vector3 direction = cameraToCenter.subtract(viewUpReal.scale(dy)).add(viewRight.scale(dx)).normalize();

						nearestT = Double.POSITIVE_INFINITY;
						nearestObject = null;

						// Try to intersect with all the objects in the scene
						for (int i = 0; i < sceneObjects.size(); i++) {
							double t = sceneObjects.get(i).intersects(viewFrom, direction);
							if (t >= 0 && t < nearestT) {
								nearestT = t;
								nearestObject = sceneObjects.get(i);
							}
						}

						if (nearestObject == null)
							pixels[x][y] = backgroundColor;
						else
							pixels[x][y] = nearestObject.getColor();
					}
					
					synchronized (Scene.this) {
						linesCompleted++;
					}
				}
			}
		}
		
		// Simple progress outputter.
		new Thread() {
			public void run() {
				while (linesCompleted < resolutionY) {
					System.err.print("\r" + new DecimalFormat("#0.00").format((double) linesCompleted / resolutionY * 100) + "%");
					
					try {
						sleep(1000);
					} catch (InterruptedException e) {
						e.printStackTrace();
					}
				}
				
				System.err.println();
			}
		}.start();
		
		// Start n threads where n is the number of available processors.
		List<Thread> threads = new ArrayList<Thread>();
		int numThreads = Runtime.getRuntime().availableProcessors();
		for (int i = 0; i < numThreads; i++)
			threads.add(new RayTraceThread(resolutionY * i / numThreads, resolutionY * (i + 1) / numThreads));
		
		for (int i = 0; i < threads.size(); i++) 
			threads.get(i).start();
		
		for (int i = 0; i < threads.size(); i++)
			try {
				threads.get(i).join();
			} catch (InterruptedException e) {
				e.printStackTrace();
			}
	}
	
	public void toPpm(PrintStream out) {
		rayTrace();
		
		// Output header.
		out.println("P6");
		out.println(resolutionY + " " + resolutionX);
		out.println("255");
		
		// Output all pixels.
		for (int y = 0; y < resolutionY; y++)
			for (int x = 0; x < resolutionX; x++) {
				Vector3 pixel = pixels[x][y];
				out.write((byte) (255 * pixel.getX()));
				out.write((byte) (255 * pixel.getY()));
				out.write((byte) (255 * pixel.getZ()));
			}
		
		out.flush();
	}
}
