package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class LogoutServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		request.getSession().removeAttribute("userData");
		String url = request.getRequestURL().toString();
		url = url.substring(0, url.lastIndexOf('/'));
		response.sendRedirect(url + "/org.thestaticvoid.cmsc484blog.ViewArticlesServlet");
	}
}
