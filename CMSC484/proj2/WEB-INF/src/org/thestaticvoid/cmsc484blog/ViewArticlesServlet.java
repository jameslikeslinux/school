package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class ViewArticlesServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		Utils.doHeader(request, response, "Home");
		request.getRequestDispatcher("/WEB-INF/jsp/viewarticles.jsp").include(request, response);
		Utils.doFooter(request, response);
	}
}
