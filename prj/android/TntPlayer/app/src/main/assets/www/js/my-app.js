var $$ = Dom7;
// Initialize your app
var myApp = new Framework7({
    onPageInit:pageInit,
});

// Export selectors engine

var ui={'login':false,'userguid':''};

// Add views
var view1 = myApp.addView('#view-1');
var view2 = myApp.addView('#view-2', {
    // Because we use fixed-through navbar we can enable dynamic navbar
    dynamicNavbar: true
});
var view3 = myApp.addView('#view-3');
var view4 = myApp.addView('#view-4');


function get_message(e){
    var that = Dom7(e);
    var lastdate = that.data('lastdate');
    var iconpath = that.data('iconpath');
    var viewtype = that.data('viewtype');
    if (typeof(lastdate) == "undefined"){  lastdate = 0;  }
    $$.ajax({
      url: 'https://gpk01.gwgz.com:666/ashx/gpk_message_get.ashx',
      data:{'topflag':lastdate,'viewtype':viewtype},
      dataType:'json',
      success: function (data, status, xhr){
        if(data.cs=="1"){
            var itemHTML="";
            if(data.rd.rows.length < 1) prompt("js://cpp?cmd=toast&arg1=获取数据动作成功！暂无最新数据！");
            $$.each(data.rd.rows, function (index, value) {
                var val = value[1];
                if(lastdate < val) lastdate = val;
                var author = '唐能通';
                itemHTML = itemHTML + '<li class="item-content" data-itemdata=\''+JSON.stringify(value)+'\'>' +
                                  '<div class="item-media"><img src="' + iconpath + '" width="44"/></div>' +
                                  '<div class="item-inner">' +
                                    '<div class="item-title-row">' +
                                      '<div class="item-title">' + value[3] + '</div>' +
                                    '</div>' +
                                    '<div class="item-subtitle">' + author + '</div>' +
                                  '</div>' +
                                '</li>';


            });
            that.find('ul').prepend(itemHTML);
            that.data('lastdate',lastdate);
        }else{ prompt("js://cpp?cmd=toast&arg1=" + data.err);  }
      },
      error:function (xhr, status){ prompt("js://cpp?cmd=toast&arg1=网络访问遇到错误(" + status + ")");  },
      complete:function (xhr, status){
        myApp.pullToRefreshDone();
      },
    });
}


// Pull to refresh content
var ptrContent = $$('.pull-to-refresh-content');
ptrContent.on('refresh', function (e) {
    get_message(e.currentTarget);
});

// Loading flag
var loading = false;
var lastIndex = 2;
var maxItems = 60;
var itemsPerLoad = 20;
// Attach 'infinite' event handler
$$('.infinite-scroll').on('infinite', function () {
  if (loading) return;
  loading = true;
  setTimeout(function () {
    loading = false;
    if (lastIndex >= maxItems) {
      myApp.detachInfiniteScroll($$('.infinite-scroll'));
      $$('.infinite-scroll-preloader').remove();
      return;
    }
    var html = '';
    for (var i = lastIndex + 1; i <= lastIndex + itemsPerLoad; i++) {
      html += '<li class="item-content"><div class="item-inner"><div class="item-title">Item ' + i + '</div></div></li>';
    }

    // Append new items
    $$('.list-block ul').append(html);
    lastIndex = $$('.list-block li').length;
  }, 1000);
});



$$('ul').on('click', '.item-content', function (e) {
    if(!ui.login){
        myApp.loginScreen();
        return;
    }
    var that = $$(this);
    var itemdata = JSON.parse(that.data("itemdata"));
    if(itemdata[5] == "92b3bc2f-124d-4c96-85e7-a81f2cdadfea"){
       prompt("js://cpp?cmd=play&type="+itemdata[5]+"&tnow="+itemdata[1]+"&pubid="+itemdata[8]);
    }

});

$$('a.login-button').on('click',function(){
    var un = $$('[name="login-username"]').val();
    var pwd = $$('[name="login-password"]').val();
    if(un=="" || pwd==""){
     prompt("js://cpp?cmd=toast&arg1=用户名和密码不能为空！");
     return;
    }
    $$.ajax({
      url: 'https://gpk01.gwgz.com:666/ashx/login.ashx',
      data:{'cmd':'login','un':un,'pwd':pwd,'type':1},
      dataType:'json',
      success: function (data, status, xhr){
        if(data.cs=="1"){
            if(data.rd.rows.length < 1){
                prompt("js://cpp?cmd=toast&arg1=用户名或密码错误！");
                return;
            }
            //$$.each(data.rd.rows, function (index, value) {
            try{
            prompt("js://cpp?cmd=login&un="+un+"&pwd="+pwd+"&userid="+data.rd.rows[0][1]);
            ui.login = true;
            ui.userguid=data.rd.rows[0][0];
           // });
            myApp.closeModal();
            }
            catch(e){
                alert(e);
            }
            return;
        }else{ prompt("js://cpp?cmd=toast&arg1=" + data.err);  }
      },
      error:function (xhr, status){
        prompt("js://cpp?cmd=toast&arg1=网络访问遇到错误(" + status + ")");
      },
      complete:function (xhr, status){
        myApp.pullToRefreshDone();
      },
    });
});