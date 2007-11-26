package org.thestaticvoid.raytracer;

public class Fill {
	private Vector3 color;
	private double diffuseComponent, specularComponent, shine, transmittance, refractionIndex;
	
	public Fill(Vector3 color, double diffuseComponent, double specularComponent, double shine, double transmittance, double refractionIndex) {
		this.color = color;
		this.diffuseComponent = diffuseComponent;
		this.specularComponent = specularComponent;
		this.shine = shine;
		this.transmittance = transmittance;
		this.refractionIndex = refractionIndex;
	}

	public Vector3 getColor() {
		return color;
	}

	public double getDiffuseComponent() {
		return diffuseComponent;
	}

	public double getRefractionIndex() {
		return refractionIndex;
	}

	public double getShine() {
		return shine;
	}

	public double getSpecularComponent() {
		return specularComponent;
	}

	public double getTransmittance() {
		return transmittance;
	}
}
