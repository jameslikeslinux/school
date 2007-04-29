package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

public class Utils {
	public static void doHeader(HttpServletRequest request, HttpServletResponse response, String title) throws ServletException, IOException {
		HttpSession session = request.getSession();
		Object username = session.getAttribute("username");

		request.setAttribute("pageTitle", new PageTitle("Home"));
		request.getRequestDispatcher("/WEB-INF/jsp/header.jsp").include(request, response);
		request.getRequestDispatcher((username == null) ? "/WEB-INF/jsp/loggedoutheader.jsp" : "/WEB-INF/jsp/loggedinheader.jsp").include(request, response);
	}

	public static void doFooter(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		request.getRequestDispatcher("/WEB-INF/jsp/footer.jsp").include(request, response);
	}
}
