package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;
import java.security.*;

/**
 * Miscellaneous utilities for use by other classes.
 *
 * @author	James Lee
 * @version	20070501
 */
public class Utils {
	/**
	 * Includes an HTML header in the current request.
	 */
	public static void doHeader(HttpServletRequest request, HttpServletResponse response, String title) throws ServletException, IOException {
		HttpSession session = request.getSession();
		Object userData = session.getAttribute("userData");

		boolean invert = false;
		if (userData != null)	// get style preferences
			try {
				invert = SqliteDb.getSingleton().isInverted(((UserData) userData).getUid());
			} catch (Exception e) {

			}

		PageTitle pageTitle = new PageTitle();
		pageTitle.setPageTitle(title);
		pageTitle.setCss(invert ? "../white.css" : "../black.css");
		request.setAttribute("pageTitle", pageTitle);

		request.getRequestDispatcher("/WEB-INF/jsp/header.jsp").include(request, response);
		request.getRequestDispatcher((userData == null) ? "/WEB-INF/jsp/loggedoutheader.jsp" : "/WEB-INF/jsp/loggedinheader.jsp").include(request, response);
	}

	/**
	 * Includes an HTML footer in the current request.
	 */
	public static void doFooter(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		request.getRequestDispatcher("/WEB-INF/jsp/footer.jsp").include(request, response);
	}

	/**
	 * Used to hash passwords for the database.
	 *
	 * @param string	Any valid string.
	 * @return		32 character hex checksum
	 */
	public static String md5(String string) {
		try {
			StringBuffer hash = new StringBuffer();
			byte[] digest = MessageDigest.getInstance("MD5").digest(string.getBytes());
			for (int i = 0; i < digest.length; i++) {
				String hex = Integer.toHexString(0xff & digest[i]);
				
				if (hex.length() == 1)
					hash.append("0");

				hash.append(hex);
			}

			return hash.toString();
		} catch (Exception e) {
			e.printStackTrace();
		}

		return "";
	}
}
