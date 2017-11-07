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

    $$.ajax({
      url: 'https://gpk01.gwgz.com:666/ashx/gpk_message_get.ashx',
      dataType:'json',
      success: function (data, status, xhr){
        if(data.cs=="1"){
            alert(data.rd.rows[0][0]);
        }
      },
      error:function (xhr, status){
      }
      ,
      statusCode: {
        404: function (xhr) {
          alert('page not found');
        }
      }
    });





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