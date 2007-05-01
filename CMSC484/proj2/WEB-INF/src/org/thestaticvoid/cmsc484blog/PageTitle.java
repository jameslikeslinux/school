package org.thestaticvoid.cmsc484blog;

/**
 * Used by servlets to create a dynamic HTML header
 *
 * @author	James Lee
 * @version	20070501
 */
public class PageTitle {
	private String pageTitle;
	private String css;

	public PageTitle(String pageTitle, String css) {
		this.pageTitle = pageTitle;
		this.css = css;
	}

	public String getPageTitle() {
		return pageTitle;
	}

	public String getCss() {
		return css;
	}
}
