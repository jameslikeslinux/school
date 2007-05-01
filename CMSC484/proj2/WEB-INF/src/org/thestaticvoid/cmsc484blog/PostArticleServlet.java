package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

/**
 * Servlet to handle all aspects of posting articles and comments.
 *
 * @author	James Lee
 * @version	20070501
 */
public class PostArticleServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		Object userData = request.getSession().getAttribute("userData");
		if (userData == null) {	// not logged in
			String url = request.getRequestURL().toString();
			url = url.substring(0, url.lastIndexOf('/'));
			response.sendRedirect(url + "/org.thestaticvoid.cmsc484blog.LoginServlet");
		}

		int aid = -1;
		try {
			aid = Integer.parseInt(request.getParameter("aid"));
		} catch (Exception e) {

		}

		String title = request.getParameter("title");
		String content = request.getParameter("content");

		Error error = new Error();
		error.setError("");
		PostArticleFormData formData = new PostArticleFormData();
		formData.setTitle((title == null) ? "" : title);
		formData.setContent((content == null) ? "" : content);

		if (content != null)	// replace some potentially bad input
			content = content.replaceAll("\n", "<br />").replaceAll("\'", "\\\'");

		try {
			SqliteDb database = SqliteDb.getSingleton();
						
			if (title == null || title.equals("") ||
			    content == null || content.equals(""))
				error.setError("All fields must be filled in.");
			else if (aid > -1) {	// making comment
				if (database.getArticle(aid) == null)
					error.setError("The specified article does not exist.");
				else {
					database.addComment(aid, ((UserData) userData).getUid(), title, content);
					String url = request.getRequestURL().toString();
					url = url.substring(0, url.lastIndexOf('/'));
					response.sendRedirect(url + "/org.thestaticvoid.cmsc484blog.ViewArticlesServlet?aid=" + aid);
				}
			} else {	// making article
				database.addArticle(((UserData) userData).getUid(), title, content);
				String url = request.getRequestURL().toString();
				url = url.substring(0, url.lastIndexOf('/'));
				response.sendRedirect(url + "/org.thestaticvoid.cmsc484blog.ViewArticlesServlet");
			}
		} catch (Exception e) {
			request.setAttribute("e", e);
			request.getRequestDispatcher("/WEB-INF/jsp/error.jsp").include(request, response);
		}

		Utils.doHeader(request, response, "Post");
		
		request.setAttribute("error", error);
		request.setAttribute("formData", formData);
		request.getRequestDispatcher("/WEB-INF/jsp/postform.jsp").include(request, response);

		Utils.doFooter(request, response);
	}

	public void doPost(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		doGet(request, response);
	}
}
