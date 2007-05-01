package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

/**
 * Servlet to handle all aspects of logging in, including displaying and 
 * processing form data.
 *
 * @author	James Lee
 * @version	20070501
 */
public class LoginServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		String username = request.getParameter("username");
		String password = request.getParameter("password");

		Error error = new Error("");
		LoginFormData formData = new LoginFormData((username == null) ? "" : username);

		try {
			SqliteDb database = SqliteDb.getSingleton();

			if (username == null || username.equals("") ||
			    password == null || password.equals(""))
				error = new Error("All fields must be filled in.");
			else if (database.isCorrectPassword(username, password)) {
				// set user data in session
				request.getSession().setAttribute("userData", new UserData(database.getUid(username), username));
				String url = request.getRequestURL().toString();
				url = url.substring(0, url.lastIndexOf('/'));
				response.sendRedirect(url + "/org.thestaticvoid.cmsc484blog.ViewArticlesServlet");
			} else
				error = new Error("Invalid username or password.");
		} catch (Exception e) {
			request.setAttribute("e", e);
			request.getRequestDispatcher("/WEB-INF/jsp/error.jsp").include(request, response);
		}

		Utils.doHeader(request, response, "Login");
		
		request.setAttribute("error", error);
		request.setAttribute("formData", formData);
		request.getRequestDispatcher("/WEB-INF/jsp/loginform.jsp").include(request, response);

		Utils.doFooter(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		doGet(request, response);
	}
}
