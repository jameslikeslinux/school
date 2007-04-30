package org.thestaticvoid.cmsc484blog;

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
