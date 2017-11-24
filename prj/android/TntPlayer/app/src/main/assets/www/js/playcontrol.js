// Initialize your app
var myApp = new Framework7({

});

// Export selectors engine
var $$ = Dom7;

$$('.demo-icon-play').on('click', function (e) {
    var that = $$(this);
    if(that.parent().hasClass('active')){
        that.parent().removeClass("active");
        prompt("js://cpp?cmd=resume");

    }else{
        that.parent().addClass('active');
        prompt("js://cpp?cmd=pause");
    }

});