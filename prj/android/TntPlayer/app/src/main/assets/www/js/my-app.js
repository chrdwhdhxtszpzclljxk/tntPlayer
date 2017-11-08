// Initialize your app
var myApp = new Framework7({
    onPageInit:pageInit,
});

// Export selectors engine
var $$ = Dom7;

// Add views
var view1 = myApp.addView('#view-1');
var view2 = myApp.addView('#view-2', {
    // Because we use fixed-through navbar we can enable dynamic navbar
    dynamicNavbar: true
});
var view3 = myApp.addView('#view-3');
var view4 = myApp.addView('#view-4');


// Dummy Content
var songs = ['Yellow Submarine', 'Don\'t Stop Me Now', 'Billie Jean', 'Californication'];
var authors = ['Beatles', 'Queen', 'Michael Jackson', 'Red Hot Chili Peppers'];

// Pull to refresh content
var ptrContent = $$('.pull-to-refresh-content');


// Add 'refresh' listener on it
ptrContent.on('refresh', function (e) {
    // Emulate 2s loading
    var that = $$(e.currentTarget);
    var lastdate = that.data('lastdate');
    var iconpath = that.data('iconpath');
    var viewtype = that.data('viewtype');
    if (typeof(lastdate) == "undefined"){
        lastdate = 0;
    }
    $$.ajax({
      url: 'https://gpk01.gwgz.com:666/ashx/gpk_message_get.ashx',
      data:{'topflag':lastdate,'viewtype':viewtype},
      dataType:'json',
      success: function (data, status, xhr){
        if(data.cs=="1"){
            //alert(data.rd.rows[0][0]);
            var itemHTML="";
            //var lastdate = 0;
            if(data.rd.rows.length < 1) prompt("js://cpp?cmd=toast&arg1=获取数据动作成功！暂无最新数据！");
            $$.each(data.rd.rows, function (index, value) {
                var val = value[1];//parseInt(value[1].replace("/Date(", "").replace(")/", ""), 10)
                if(lastdate < val){
                    lastdate = val;
                }
                // Random image
                var picURL = iconpath;
                // Random song
                var song = value[3];
                // Random author
                var author = '唐能通';
                // List item html
                itemHTML = itemHTML + '<li class="item-content">' +
                                  '<div class="item-media"><img src="' + picURL + '" width="44"/></div>' +
                                  '<div class="item-inner">' +
                                    '<div class="item-title-row">' +
                                      '<div class="item-title">' + song + '</div>' +
                                    '</div>' +
                                    '<div class="item-subtitle">' + author + '</div>' +
                                  '</div>' +
                                '</li>';


            });
            // Prepend new list element
            that.find('ul').prepend(itemHTML);
            that.data('lastdate',lastdate);

        }else{
            prompt("js://cpp?cmd=toast&arg1=" + data.err);
        }
      },
      error:function (xhr, status){
      },
      complete:function (xhr, status){
        // When loading done, we need to reset it
        myApp.pullToRefreshDone();
      },
    });



return;

    setTimeout(function () {
        // Random image
        var picURL = 'https://gpk01.gwgz.com:666/images/tnt/avter-200.jpg';
        // Random song
        var song = +songs[Math.floor(Math.random() * songs.length)];
        // Random author
        var author = authors[Math.floor(Math.random() * authors.length)];
        // List item html
        var itemHTML = '<li class="item-content">' +
                          '<div class="item-media"><img src="' + picURL + '" width="44"/></div>' +
                          '<div class="item-inner">' +
                            '<div class="item-title-row">' +
                              '<div class="item-title">' + song + '</div>' +
                            '</div>' +
                            '<div class="item-subtitle">' + author + '</div>' +
                          '</div>' +
                        '</li>';
        // Prepend new list element
        ptrContent.find('ul').prepend(itemHTML);
        // When loading done, we need to reset it
        myApp.pullToRefreshDone();
    }, 2000);
});