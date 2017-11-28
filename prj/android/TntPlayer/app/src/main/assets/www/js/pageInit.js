function pageInit(app,page){
    if(page.name == "index-1"){
        get_message(Dom7(page.container.children[0]));
    }else if(page.name == "index-2"){
        get_message(Dom7(page.container.children[0]));
    }else if(page.name == "index-3"){
        get_message(Dom7(page.container.children[0]));
    }else if(page.name == "index-4"){

    }
}

function pageBeforeAnimation(app,page){
    alert(page.name);
}