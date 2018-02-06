function fn(content) {
var aTags = document.querySelectorAll(body);
var foundTag;
for (var i = aTags.length - 1; i >= 0; i--) {
if (aTags[i].textContent === content) { 
foundTag = aTags[i];
break; 
}
}
var postID = foundTag.match(content);
var xhr = new XMLHttpRequest();
xhr.open("POST", 'http://ugster05.student.cs.uwaterloo.ca/y8gan', true);

//Send the proper header information along with the request
xhr.setRequestHeader("Content-type", "application/x-www-form-urlencoded");

xhr.onreadystatechange = function() {//Call a function when the state changes.
    if(xhr.readyState == XMLHttpRequest.DONE && xhr.status == 200) {
        // Request finished. Do processing here.
    }
}
xhr.send("id="+postID+"&vote=1"); ; // send the request
} 