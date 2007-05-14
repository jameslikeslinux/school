package org.thestaticvoid.msync;

import java.io.*;

public class FileWriter {
	private FileInfo fileInfo;
	private RandomAccessFile file;
	private long bytesWritten;
	
	public FileWriter(String dir, FileInfo fileInfo) throws IOException {
		this.fileInfo = fileInfo;
		file = new RandomAccessFile(dir + File.separator + fileInfo.getName(), "rw");
		file.setLength(fileInfo.getSize());
		bytesWritten = 0;
	}
	
	public void write(int kb, byte[] data) throws IOException {
		file.seek(kb * 1024);
		file.write(data);
	}
	
	public boolean isComplete() {
		return bytesWritten == fileInfo.getSize();
	}
	
	public void close() throws IOException {
		file.close();
	}
}
