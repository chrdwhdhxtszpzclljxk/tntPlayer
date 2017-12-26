//
//  ViewController.m
//  tntPlayer
//
//  Created by zcl on 2017/12/22.
//  Copyright © 2017年 zcl. All rights reserved.
//

#import "ViewController.h"
#import <UIKit/UIKit.h>
#import <WebKit/WebKit.h>

@interface ViewController () <WKNavigationDelegate>

@end

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    
    WKWebViewConfiguration *config = [[WKWebViewConfiguration alloc] init];
    // 设置偏好设置
    config.preferences = [[WKPreferences alloc] init];
    // 默认为0
    config.preferences.minimumFontSize = 10;
    // 默认认为YES
    config.preferences.javaScriptEnabled = YES;
    // 在iOS上默认为NO，表示不能自动通过窗口打开
    config.preferences.javaScriptCanOpenWindowsAutomatically = NO;
    // web内容处理池，由于没有属性可以设置，也没有方法可以调用，不用手动创建
    config.processPool = [[WKProcessPool alloc] init];
    // 通过JS与webview内容交互
    config.userContentController = [[WKUserContentController alloc] init];
    
    // 注入JS对象名称AppModel，当JS通过AppModel来调用时，
    // 我们可以在WKScriptMessageHandler代理中接收到
    [config.userContentController addScriptMessageHandler:self name:@"AppModel"];
    
    
    // 创建WKWebView
    //WKWebView *webView = [[WKWebView alloc] initWithFrame:[UIScreen mainScreen].bounds];
    WKWebView * webView = [[WKWebView alloc] initWithFrame:CGRectMake(0,0, self.view.frame.size.width,self.view.frame.size.height-0) configuration:config];
    webView.scrollView.backgroundColor = [UIColor blackColor];
    webView.navigationDelegate = self;    // 设置访问的URL
     
    NSURL *url = [NSURL URLWithString:@"https://www.jianshu.com"];
    // 根据URL创建请求
    NSURLRequest *request = [NSURLRequest requestWithURL:url];
    // WKWebView加载请求
    [webView loadRequest:request];
    // 将WKWebView添加到视图
    [self.view addSubview:webView];
    
    
}


- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


@end
