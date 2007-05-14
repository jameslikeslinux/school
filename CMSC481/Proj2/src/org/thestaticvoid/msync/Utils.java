package org.thestaticvoid.msync;

import java.io.*;
import java.security.*;

public class Utils {
	
	public static String md5(File file) {
		try {
			MessageDigest digest = MessageDigest.getInstance("MD5");
			InputStream input = new FileInputStream(file);
			byte[] buffer = new byte[4092];
			int read;
			while ((read = input.read(buffer)) > -1)
				digest.update(buffer, 0, read);
			input.close();
			
			byte[] hash = digest.digest();
			StringBuffer hashStr = new StringBuffer();
			for (int i = 0; i < hash.length; i++) {
				String hex = Integer.toHexString(0xff & hash[i]);
				
				if (hex.length() == 1)
					hashStr.append("0");
				
				hashStr.append(hex);
			}
			
			return hashStr.toString();
		} catch (Exception e) {
			e.printStackTrace();
		}
		
		return "";
	}
}
