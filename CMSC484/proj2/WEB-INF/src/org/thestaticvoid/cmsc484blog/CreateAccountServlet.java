package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class CreateAccountServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		String username = request.getParameter("username");
		String password1 = request.getParameter("password1");
		String password2 = request.getParameter("password2");
		String name = request.getParameter("name");

		Error error = new Error("");
		CreateAccountFormData formData = new CreateAccountFormData((username == null) ? "" : "" + username, (name == null) ? "" : "" + name);


		try {
			SqliteDb database = SqliteDb.getSingleton();

			if (username == null || username.equals("") ||
			    password1 == null || password1.equals("") ||
			    password2 == null || password2.equals("") ||
			    name == null || name.equals(""))
				error = new Error("All fields must be filled in.");
			else if (!password1.equals(password2))
				error = new Error("The passwords are not equal.");
			else if (!database.isUsernameAvailable(username)) 
				error = new Error("The requested username already exists.");
			else
				database.createUser(username, password1, name);
		} catch (Exception e) {
			error = new Error(e.getMessage());
		}

		Utils.doHeader(request, response, "Home");
		
		request.setAttribute("error", error);
		request.setAttribute("formData", formData);
		request.getRequestDispatcher("/WEB-INF/jsp/createaccountform.jsp").include(request, response);

		Utils.doFooter(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		doGet(request, response);
	}
}
