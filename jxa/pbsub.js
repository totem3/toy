#!/usr/bin/osascript -l JavaScript

ObjC.import('stdlib');
function usage() {
  console.log("index.js /pattern/replace[/flags]");
}
function run(args) {
  if (args[0] === undefined) {
    usage();
    $.exit(1);
  }
  var [_, patstr, rep, flags] = args[0].split("/");
  var app = Application.currentApplication();
  app.includeStandardAdditions = true;
  var cb = app.theClipboard();
  // console.log("before: " + cb);
  if (typeof cb != "string") {
    console.log("could not get clipboard content");
    $.exit(1);
  }
  var regex = new RegExp(patstr, flags);
  var ncb = cb.replace(regex, rep);
  // console.log("after: " + ncb);
  app.setTheClipboardTo(ncb);
  $.exit(0);
}

