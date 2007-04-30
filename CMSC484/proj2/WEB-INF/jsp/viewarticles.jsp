<%@ page contentType="text/html; charset=UTF-8" language="java" import="org.thestaticvoid.cmsc484blog.*" %>
<%
	Article[] articles = (Article[]) request.getAttribute("articles");
	for (int i = articles.length - 1; i >= 0; i--) {
%>
		<a href="org.thestaticvoid.cmsc484blog.ViewArticlesServlet?aid=<%= articles[i].getId() %>"><h4><%= articles[i].getTitle() %></h4></a>
		<h5><%= articles[i].getUsername() %></h5>
		<h6><%= articles[i].getTimestamp() %></h6>
		<p><%= articles[i].getContent() %></p>
<%
	}
%>
