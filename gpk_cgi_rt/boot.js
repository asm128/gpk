function bootstrap(name) {
    var url = [self.location.protocol, '//', self.location.host, self.location.pathname].join('');
    self.location = url + "?m=test_cgi_module&bt=1" + "&width=" + document.documentElement.clientWidth + "&height=" + document.documentElement.clientHeight;
} 