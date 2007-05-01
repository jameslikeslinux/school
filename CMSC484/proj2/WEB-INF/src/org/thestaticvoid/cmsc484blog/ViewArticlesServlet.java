package org.thestaticvoid.cmsc484blog;

import javax.servlet.*;
import javax.servlet.http.*;
import java.io.*;

/**
 * Main page servlet shows all articles in the database.  Also accepts
 * <code>aid</code> parameter to show only one article and its comments.
 *
 * @author	James Lee
 * @version	20070501
 */
public class ViewArticlesServlet extends HttpServlet {
	public void doGet(HttpServletRequest request, HttpServletResponse response) throws ServletException, IOException {
		int aid = -1;
		try {
			aid = Integer.parseInt(request.getParameter("aid"));
		} catch (Exception e) {

		}

		SqliteDb database = null;
		try {
			database = SqliteDb.getSingleton();


			if (aid > -1) {	// view article and comments
				Article article = database.getArticle(aid);
				if (article == null) {
					Utils.doHeader(request, response, "Bad article");
					request.getRequestDispatcher("/WEB-INF/jsp/invalidarticle.jsp").include(request, response);
				} else {
					Utils.doHeader(request, response, article.getTitle());
					request.setAttribute("article", article);
					request.getRequestDispatcher("/WEB-INF/jsp/viewarticle.jsp").include(request, response);
				}
			} else {	// view all articles
				Utils.doHeader(request, response, "Home");
				request.setAttribute("articles", database.getAllArticles());
				request.getRequestDispatcher("/WEB-INF/jsp/viewarticles.jsp").include(request, response);
			}

			Utils.doFooter(request, response);
		} catch (Exception e) {
			request.setAttribute("e", e);
			request.getRequestDispatcher("/WEB-INF/jsp/error.jsp").include(request, response);
		}
	}
}
