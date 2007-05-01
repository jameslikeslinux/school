package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

/**
 * Servlet to handle all aspects of showing and setting user's preferences.
 *
 * @author	James Lee
 * @version	20070501
 */
public class PreferencesServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		Object userData = request.getSession().getAttribute("userData");
		if (userData == null) {
			String url = request.getRequestURL().toString();
			url = url.substring(0, url.lastIndexOf('/'));
			response.sendRedirect(url + "/org.thestaticvoid.cmsc484blog.LoginServlet");
		}

		boolean invert = false;
		try {
			SqliteDb database = SqliteDb.getSingleton();
			if (request.getParameter("submit") != null) {
				invert = request.getParameter("invert") != null;
				database.setInverted(((UserData) userData).getUid(), invert);
			} else
				invert = database.isInverted(((UserData) userData).getUid());
		} catch (Exception e) {
			request.setAttribute("e", e);
			request.getRequestDispatcher("/WEB-INF/jsp/error.jsp").include(request, response);
		}

		Utils.doHeader(request, response, "Preferences");
		request.setAttribute("formData", new PreferencesFormData(invert));
		request.getRequestDispatcher("/WEB-INF/jsp/preferencesform.jsp").include(request, response);
		Utils.doFooter(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		doGet(request, response);
	}
}
