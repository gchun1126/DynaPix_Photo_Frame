#pragma once

#include <Arduino.h>

// Generated DynaPix embedded web interface.
// Keep this file beside DynaPix_Photo_Frame.ino.

const char INDEX_HTML[] PROGMEM = R"DPIXUI26(
<!doctype html>
<html lang="en">
<head>
<meta charset="utf-8">
<meta name="viewport" content="width=device-width,initial-scale=1,maximum-scale=1">
<meta name="theme-color" content="#0a5961">

<link
 rel="icon"
 type="image/svg+xml"
 href="/favicon.svg?v=2"
>

<link
 rel="shortcut icon"
 href="/favicon.svg?v=2"
>

<meta
 name="application-name"
 content="DynaPix"
>

<meta
 name="apple-mobile-web-app-title"
 content="DynaPix"
>

<title>DynaPix E-Paper Studio</title>

<style>

@font-face{
 font-family:"DynaPix Audiowide";
 src:url("/assets/audiowide-regular.woff2?v=3")
     format("woff2");
 font-weight:400;
 font-style:normal;
 font-display:swap
}
@font-face{
 font-family:"DynaPix Montserrat";
 src:url("/assets/montserrat-bold.woff2?v=3")
     format("woff2");
 font-weight:700;
 font-style:normal;
 font-display:swap
}
@font-face{
 font-family:"DynaPix Inter";
 src:url("/assets/inter-regular.woff2?v=3")
     format("woff2");
 font-weight:400;
 font-style:normal;
 font-display:swap
}
@font-face{
 font-family:"DynaPix Inter";
 src:url("/assets/inter-medium.woff2?v=3")
     format("woff2");
 font-weight:500;
 font-style:normal;
 font-display:swap
}
@font-face{
 font-family:"DynaPix Inter";
 src:url("/assets/inter-semibold.woff2?v=3")
     format("woff2");
 font-weight:600;
 font-style:normal;
 font-display:swap
}

:root{
 color-scheme:dark;
 --bg:#090d14;
 --card:#121925;
 --card2:#192334;
 --border:#2d3a50;
 --text:#f5f7fb;
 --muted:#94a3b8;
 --green:#2bd8aa;
 --blue:#587fff;
 --red:#ff6670;
 --yellow:#ffc04f;
 --nav:72px
}
*{box-sizing:border-box}
body{
 margin:0;
 background:radial-gradient(circle at 15% 0,#192740,transparent 35%),var(--bg);
 color:var(--text);
 font-family:
  "DynaPix Inter",
  system-ui,
  -apple-system,
  sans-serif
}
button,input,select{font:inherit}
button{cursor:pointer}
button:disabled{opacity:.42;cursor:not-allowed}
.page{width:min(1450px,100%);margin:auto;padding:18px}
header{
 display:flex;justify-content:space-between;align-items:center;
 gap:12px;margin-bottom:18px
}
.brand{display:flex;align-items:center;gap:12px;min-width:0}
.brand-mark{
 display:block;
 width:56px;
 height:56px;
 flex:0 0 auto;
 overflow:visible;

 filter:
  drop-shadow(
   0 7px 17px rgba(10,89,97,.24)
  )
}
.brand-title{
 display:block;max-width:460px;overflow:hidden;
 text-overflow:ellipsis;white-space:nowrap;
 font-family:"DynaPix Audiowide",system-ui,sans-serif;
 font-size:24px;font-weight:400;letter-spacing:-.04em
}
.small,.muted{color:var(--muted);font-size:12px}
.connection{
 padding:9px 13px;border:1px solid var(--border);
 border-radius:999px;background:var(--card);font-size:12px
}
nav{
 display:flex;gap:6px;width:max-content;margin-bottom:18px;
 padding:5px;border-radius:13px;background:#0d131d
}
nav button{
 min-width:130px;padding:10px 18px;border:0;border-radius:9px;
 color:var(--muted);background:transparent;font-weight:750
}
nav button.active{color:var(--text);background:var(--card2)}
.view{display:none}
.view.active{display:block}
.grid{
 display:grid;grid-template-columns:390px minmax(0,1fr);
 gap:18px;align-items:start
}
.card{
 overflow:hidden;border:1px solid var(--border);
 border-radius:17px;background:rgba(18,25,37,.96)
}
.section{padding:18px;border-bottom:1px solid var(--border)}
.section:last-child{border-bottom:0}
h2,h3{margin:0 0 8px}
h2{font-size:19px}
h3{font-size:14px}
.drop{
 display:grid;place-items:center;min-height:135px;padding:18px;
 text-align:center;border:1.5px dashed #455978;
 border-radius:13px;background:#0e1520
}
.drop.drag{border-color:var(--green)}
#fileInput{display:none}
.primary,.secondary,.danger,.ghost{
 padding:10px 13px;border:0;border-radius:10px;font-weight:800
}
.primary{color:#06120f;background:var(--green)}
.secondary{color:var(--text);background:#29364b}
.danger{color:#ffd9dc;background:#542d36}
.ghost{color:var(--muted);background:transparent}
.full{width:100%}
.row{display:flex;align-items:center;gap:8px;flex-wrap:wrap}
.between{justify-content:space-between}
.field{margin-top:13px}
label{
 display:block;margin-bottom:6px;color:#d5deec;
 font-size:12px;font-weight:700
}
input[type=text],input[type=password],input[type=number],select{
 width:100%;min-height:42px;padding:8px 11px;color:var(--text);
 background:#0d141e;border:1px solid #35445c;border-radius:9px
}
input[type=range]{width:100%;accent-color:var(--green)}
.checkline{
 display:flex;align-items:flex-start;gap:9px;
 margin-top:10px;font-size:12px;font-weight:650
}
.checkline input{margin-top:2px;accent-color:var(--green)}
details{
 margin-top:10px;border:1px solid var(--border);
 border-radius:11px;background:#0e1520
}
summary{padding:13px;cursor:pointer;font-weight:800;font-size:13px}
.details-body{padding:0 13px 13px}
.pending-strip{
 display:flex;gap:8px;margin-top:12px;padding-bottom:5px;
 overflow:auto;scroll-snap-type:x mandatory
}
.pending-chip{
 flex:0 0 115px;padding:9px;border:1px solid var(--border);
 border-radius:9px;color:var(--text);background:#111a28;
 text-align:left;scroll-snap-align:start
}
.pending-chip.active{border-color:var(--blue);background:#20304a}
.pending-chip strong,.pending-chip span{
 display:block;overflow:hidden;text-overflow:ellipsis;white-space:nowrap
}
.pending-chip span{margin-top:4px;color:var(--muted);font-size:10px}
.canvas-frame{
 position:relative;width:100%;aspect-ratio:3/2;overflow:hidden;
 background:white;border:1px solid #536078;border-radius:13px
}
canvas.preview{display:block;width:100%;height:100%}
#sourceCanvas{cursor:grab;touch-action:none}
.empty{
 position:absolute;inset:0;display:grid;place-items:center;
 padding:20px;color:#65748a;background:white;text-align:center
}
.preview-wrap{padding:18px}
.tabs{
 display:flex;gap:4px;padding:4px;border-radius:10px;background:#0d141e
}
.tabs button{
 padding:8px 12px;border:0;border-radius:7px;
 color:var(--muted);background:transparent
}
.tabs button.active{color:var(--text);background:var(--card2)}
.status{
 margin-top:14px;padding:14px;border:1px solid var(--border);
 border-radius:12px;background:#0d141e
}
.progress{
 height:7px;margin:10px 0;overflow:hidden;
 border-radius:99px;background:#263246
}
.progress div{
 width:0;height:100%;
 background:linear-gradient(90deg,var(--blue),var(--green))
}
.batch-actions{
 display:grid;grid-template-columns:1fr 1fr;
 gap:8px;margin-top:10px
}
.slideshow-card{
 padding:17px;
 background:linear-gradient(135deg,rgba(88,127,255,.13),transparent),var(--card)
}
.stats{
 display:grid;grid-template-columns:repeat(3,1fr);
 gap:10px;margin-top:12px
}
.stat{
 padding:13px;border:1px solid var(--border);
 border-radius:11px;background:#0d141e
}
.stat strong{
 display:block;margin-top:3px;overflow:hidden;
 text-overflow:ellipsis;white-space:nowrap;font-size:17px
}
.library-toolbar{padding:15px}
.library{
 display:grid;grid-template-columns:repeat(auto-fill,minmax(205px,1fr));
 gap:13px;padding:15px
}
.photo{
 position:relative;overflow:hidden;border:1px solid var(--border);
 border-radius:13px;background:var(--card)
}
.photo.selected-card{border-color:var(--green)}
.photo canvas{
 display:block;width:100%;aspect-ratio:3/2;
 background:white;cursor:pointer
}
.photo-body{padding:10px}
.photo-name{
 overflow:hidden;text-overflow:ellipsis;white-space:nowrap;
 font-size:13px;font-weight:800
}
.photo-meta{margin-top:4px;color:var(--muted);font-size:10px}
.menu-button{
 position:absolute;top:7px;right:7px;width:34px;height:34px;
 padding:0;border:1px solid rgba(255,255,255,.15);
 border-radius:50%;color:white;background:rgba(8,12,18,.8)
}
.select-box{
 position:absolute;top:8px;left:8px;width:25px;height:25px;
 accent-color:var(--green)
}
.reorder-list{padding:15px}
.reorder-row{
 display:grid;grid-template-columns:1fr auto auto;
 gap:7px;align-items:center;padding:11px;
 border-bottom:1px solid var(--border)
}
.selection-bar{
 position:sticky;bottom:82px;z-index:20;display:none;
 align-items:center;justify-content:space-between;gap:8px;
 margin:12px;padding:12px;border:1px solid var(--border);
 border-radius:13px;background:#172235;box-shadow:0 8px 30px #0008
}
.selection-bar.visible{display:flex}
.settings-grid{
 display:grid;grid-template-columns:1fr 1fr;
 gap:18px;align-items:start
}
.warning{margin-top:6px;color:var(--yellow);font-size:11px}
.storage-bar{
 height:9px;margin-top:9px;overflow:hidden;
 border-radius:99px;background:#263246
}
.storage-bar div{height:100%;background:var(--green)}
.overlay{
 position:fixed;inset:0;z-index:100;display:none;
 align-items:flex-end;justify-content:center;padding:18px;
 background:rgba(0,0,0,.68)
}
.overlay.visible{display:flex}
.modal{
 width:min(520px,100%);max-height:88vh;overflow:auto;
 border:1px solid var(--border);border-radius:17px;
 background:#121925;box-shadow:0 30px 80px #000
}
.modal-header{
 display:flex;align-items:center;justify-content:space-between;
 gap:10px;padding:15px;border-bottom:1px solid var(--border)
}
.modal-body{padding:15px}
.modal-actions{display:grid;gap:8px}
.queue-list{max-height:60vh;overflow:auto}
.queue-row{
 display:flex;justify-content:space-between;gap:10px;
 padding:10px;border-bottom:1px solid var(--border);font-size:12px
}

.category-controls{
 display:flex;align-items:center;gap:9px;
 padding:0 15px 15px;flex-wrap:wrap
}
.category-controls select{
 width:auto;min-width:220px
}
.category-chips{
 display:flex;gap:5px;flex-wrap:wrap;margin-top:7px
}
.category-chip{
 display:inline-flex;align-items:center;gap:5px;
 padding:3px 7px;border-radius:999px;
 color:#eef6f5;background:#203039;font-size:10px
}
.category-dot{
 width:8px;height:8px;border-radius:50%;flex:0 0 auto
}
.category-list{
 display:grid;gap:9px
}
.category-row{
 display:grid;
 grid-template-columns:42px minmax(0,1fr) auto auto;
 gap:7px;align-items:center;padding:9px;
 border:1px solid var(--border);border-radius:10px
}
.category-row input[type=color]{
 width:38px;height:38px;padding:2px;border:0;
 border-radius:8px;background:transparent
}
.category-options{
 display:grid;gap:8px
}
.category-option{
 display:flex;align-items:center;gap:9px;
 padding:9px;border:1px solid var(--border);
 border-radius:9px;background:#0e1520
}
.category-option input{
 accent-color:var(--green)
}
.slideshow-category-list{
 display:grid;
 grid-template-columns:repeat(auto-fit,minmax(150px,1fr));
 gap:7px;margin-top:8px
}


.extensions-grid{
 display:grid;
 grid-template-columns:repeat(auto-fit,minmax(240px,1fr));
 gap:15px
}
.extension-card{
 padding:18px;border:1px solid var(--border);
 border-radius:15px;background:var(--card)
}
.extension-icon{
 font-size:36px;margin-bottom:10px
}
.extension-dashboard{
 display:grid;
 grid-template-columns:minmax(280px,390px) minmax(0,1fr);
 gap:18px;align-items:start
}
.extension-preview{
 padding:16px
}
.extension-status{
 display:grid;
 grid-template-columns:repeat(3,1fr);
 gap:8px;margin-top:12px
}
.extension-status-box{
 padding:11px;border:1px solid var(--border);
 border-radius:10px;background:#0d141e
}
.extension-status-box strong{
 display:block;margin-top:4px
}
@media(max-width:850px){
 .extension-dashboard{
  grid-template-columns:1fr
 }
 .extension-preview{
  order:-1
 }
 .extension-status{
  grid-template-columns:1fr
 }
}


.calendar-dashboard{
 display:grid;
 grid-template-columns:minmax(310px,420px) minmax(0,1fr);
 gap:18px;
 align-items:start
}

.calendar-preview{
 padding:16px
}

.calendar-feed{
 margin-top:12px;
 padding:12px;
 border:1px solid var(--border);
 border-radius:11px;
 background:#0e1520
}

.calendar-feed-grid{
 display:grid;
 grid-template-columns:75px minmax(0,1fr) 48px;
 gap:8px;
 align-items:end
}

.calendar-state{
 display:grid;
 grid-template-columns:repeat(3,1fr);
 gap:8px;
 margin-top:12px
}

.calendar-state div{
 padding:11px;
 border:1px solid var(--border);
 border-radius:10px;
 background:#0d141e
}

.calendar-state strong{
 display:block;
 margin-top:4px
}

@media(max-width:850px){
 .calendar-dashboard{
  grid-template-columns:1fr
 }

 .calendar-preview{
  order:-1
 }

 .calendar-feed-grid{
  grid-template-columns:65px minmax(0,1fr)
 }

 .calendar-state{
  grid-template-columns:1fr
 }
}

.hidden{display:none!important}
@media(max-width:850px){
 body{padding-bottom:calc(var(--nav) + 12px)}
 .page{padding:11px}
 header{align-items:flex-start}
 .brand-title{max-width:185px}
 .connection{
  max-width:145px;overflow:hidden;
  text-overflow:ellipsis;white-space:nowrap
 }
 nav{
  position:fixed;left:10px;right:10px;bottom:9px;z-index:80;
  width:auto;height:var(--nav);margin:0;padding:6px;
  border:1px solid var(--border);box-shadow:0 15px 50px #000
 }
 nav button{flex:1;min-width:0;padding:7px 5px;font-size:12px}
 .grid,.settings-grid{grid-template-columns:1fr}
 .preview-wrap{padding:12px}
 .library{
  grid-template-columns:repeat(2,minmax(0,1fr));
  padding:10px;gap:9px
 }
 .library-toolbar{align-items:flex-start}
 .stats{grid-template-columns:1fr}
 .batch-actions{grid-template-columns:1fr}
 .selection-bar{
  align-items:stretch;flex-direction:column;
  bottom:82px
 }
 .overlay{padding:8px}
 .modal{border-radius:17px 17px 8px 8px}
}
</style>
</head>

<body>
<div class="page">

<header>
 <div class="brand">
  <svg
   class="brand-mark"
   viewBox="0 0 72 72"
   xmlns="http://www.w3.org/2000/svg"
   role="img"
   aria-label="DynaPix"
  >
   <defs>
    <linearGradient
     id="dynapixTeal"
     x1="0"
     y1="0"
     x2="1"
     y2="1"
    >
     <stop offset="0" stop-color="#16808a"/>
     <stop offset="1" stop-color="#063f45"/>
    </linearGradient>

    <linearGradient
     id="dynapixCoral"
     x1="0"
     y1="0"
     x2="1"
     y2="1"
    >
     <stop offset="0" stop-color="#ff735d"/>
     <stop offset="1" stop-color="#fe5945"/>
    </linearGradient>
   </defs>

   <path
    d="M11 9h39a5 5 0 0 1 5 5v6h-5v-5a1 1 0 0 0-1-1H15a4 4 0 0 0-4 4v36a4 4 0 0 0 4 4h33a2 2 0 0 0 2-2v-4h5v5a6 6 0 0 1-6 6H15A9 9 0 0 1 6 54V18a9 9 0 0 1 5-9Z"
    fill="url(#dynapixTeal)"
   />

   <path
    d="M17 17h30a2 2 0 0 1 2 2v3h-5v-1H18a3 3 0 0 0-3 3v25l13-13a3 3 0 0 1 4 0l8 8 5-5a3 3 0 0 1 4 0l6 6v7H18a8 8 0 0 1-8-8V24a7 7 0 0 1 7-7Z"
    fill="url(#dynapixTeal)"
   />

   <circle
    cx="24"
    cy="28"
    r="4.7"
    fill="url(#dynapixCoral)"
   />

   <g fill="url(#dynapixTeal)">
    <rect x="53" y="17" width="6" height="6" rx="1"/>
    <rect x="60" y="11" width="4" height="4" rx=".8"/>
    <rect x="58" y="27" width="5" height="5" rx=".8"/>
    <rect x="50" y="31" width="4" height="4" rx=".8"/>
    <rect x="63" y="35" width="5" height="5" rx=".8"/>
    <rect x="55" y="42" width="5" height="5" rx=".8"/>
    <rect x="62" y="50" width="4" height="4" rx=".8"/>
   </g>

   <g fill="url(#dynapixCoral)">
    <rect x="63" y="20" width="5" height="5" rx=".8"/>
    <rect x="66" y="29" width="4" height="4" rx=".8"/>
    <rect x="57" y="36" width="4" height="4" rx=".8"/>
    <rect x="66" y="43" width="5" height="5" rx=".8"/>
    <rect x="58" y="55" width="4" height="4" rx=".8"/>
   </g>
  </svg>
  <div>
   <strong id="deviceTitle" class="brand-title">DynaPix</strong>
   <span class="small">E-Paper Studio</span>
  </div>
 </div>
 <div id="connection" class="connection">Connecting…</div>
</header>

<nav>
 <button class="nav active" data-view="addView">＋ Add Photos</button>
 <button class="nav" data-view="libraryView">▣ Library</button>
 <button class="nav" data-view="extensionsView">◷ Extensions</button>
 <button class="nav" data-view="settingsView">⚙ Settings</button>
</nav>

<section id="addView" class="view active">
 <div class="grid">
  <aside class="card">

   <div class="section">
    <h3>Add photos</h3>

    <p class="muted">
     Select multiple photos. HEIC support depends on the browser.
     Current iPhone Safari is recommended.
    </p>

    <div id="dropZone" class="drop">
     <div>
      <strong>Select or drop photos</strong><br>
      <span class="small">
       HEIC, HEIF, JPG, PNG, WebP, BMP, AVIF
      </span>
     </div>
    </div>

    <input
     id="fileInput"
     type="file"
     multiple
     accept=".heic,.heif,.jpg,.jpeg,.png,.webp,.bmp,.avif,image/heic,image/heif,image/jpeg,image/png,image/webp,image/bmp,image/avif"
    >

    <div id="pendingStrip" class="pending-strip"></div>

    <div class="row between" style="margin-top:10px">
     <button id="previousPending" class="secondary">Previous</button>

     <button id="openQueue" class="ghost">
      Queue: <span id="queuePosition">0</span>
     </button>

     <button id="nextPending" class="secondary">Next</button>
    </div>
   </div>

   <div class="section">

    <details open>
     <summary>Current photo adjustments</summary>

     <div class="details-body">
      <div class="field">
       <label>
        Zoom <span id="zoomValue">100%</span>
       </label>

       <input
        id="zoom"
        type="range"
        min="100"
        max="300"
        value="100"
       >
      </div>

      <p class="small">
       Drag directly inside the preview to position the photo.
      </p>

      <div class="row">
       <button id="rotateLeft" class="secondary">↶ Rotate left</button>
       <button id="rotateRight" class="secondary">↷ Rotate right</button>
       <button id="removePending" class="danger">Remove</button>
      </div>
     </div>
    </details>

    <details open>
     <summary>Photo quality</summary>

     <div class="details-body">
      <div class="field">
       <label>Quality preset</label>

       <select id="qualityPreset">
        <option value="vivid">Vivid — bright and colorful</option>
        <option value="natural">Natural — accurate and balanced</option>
        <option value="portrait">Portrait — skin and faces</option>
        <option value="graphics">Graphics — logos and text</option>
        <option value="detail" selected>Maximum detail</option>
       </select>
      </div>

      <div class="field">
       <label>Fit</label>

       <select id="fitMode">
        <option value="fill">Fill and crop</option>
        <option value="contain">Fit entire photo</option>
        <option value="stretch">Stretch</option>
       </select>
      </div>

      <label class="checkline">
       <input id="autoOptimize" type="checkbox" checked>
       <span>Automatic tone and gentle white-balance optimization</span>
      </label>

      <label class="checkline">
       <input id="protectHighlights" type="checkbox" checked>
       <span>Protect bright highlights</span>
      </label>

      <label class="checkline">
       <input id="enhanceColor" type="checkbox" checked>
       <span>Enhance weak colors with vibrance</span>
      </label>

      <label class="checkline">
       <input id="sharpenEnabled" type="checkbox" checked>
       <span>Sharpen after resizing</span>
      </label>

      <label class="checkline">
       <input id="skinProtection" type="checkbox" checked>
       <span>Protect skin and warm neutral tones</span>
      </label>

      <div class="field">
       <label>
        Vivid strength <span id="vividStrengthValue">46</span>
       </label>

       <input
        id="vividStrength"
        type="range"
        min="0"
        max="100"
        value="46"
       >
      </div>

      <div class="field">
       <label>Dithering</label>

       <select id="ditherMode">
        <option value="smart" selected>Smart serpentine</option>
        <option value="atkinson">Bright Atkinson</option>
        <option value="clean">Clean — no dithering</option>
       </select>
      </div>
     </div>
    </details>

    <details>
     <summary>Advanced quality controls</summary>

     <div class="details-body">
      <div class="field">
       <label>
        Brightness <span id="brightnessValue">+2</span>
       </label>

       <input id="brightness" type="range" min="-40" max="40" value="2">
      </div>

      <div class="field">
       <label>
        Contrast <span id="contrastValue">+8</span>
       </label>

       <input id="contrast" type="range" min="-40" max="40" value="8">
      </div>

      <div class="field">
       <label>
        Saturation <span id="saturationValue">0</span>
       </label>

       <input id="saturation" type="range" min="-50" max="50" value="0">
      </div>

      <div class="field">
       <label>
        Vibrance <span id="vibranceValue">+16</span>
       </label>

       <input id="vibrance" type="range" min="-20" max="60" value="16">
      </div>

      <div class="field">
       <label>
        Shadow detail <span id="shadowDetailValue">+8</span>
       </label>

       <input id="shadowDetail" type="range" min="0" max="30" value="8">
      </div>

      <div class="field">
       <label>
        Highlight protection <span id="highlightDetailValue">22</span>
       </label>

       <input id="highlightDetail" type="range" min="0" max="40" value="22">
      </div>

      <div class="field">
       <label>
        Sharpness <span id="sharpnessValue">30</span>
       </label>

       <input id="sharpness" type="range" min="0" max="60" value="30">
      </div>

      <div class="field">
       <label>
        Dither strength <span id="ditherStrengthValue">84</span>
       </label>

       <input id="ditherStrength" type="range" min="0" max="100" value="84">
      </div>

      <div class="field">
       <label>
        Color style <span id="colorStyleValue">44</span>
       </label>

       <input id="colorStyle" type="range" min="0" max="100" value="44">

       <div class="small">
        Lower values favor neutral accuracy. Higher values favor
        stronger colored ink.
       </div>
      </div>

      <div class="field">
       <label>Edge protection</label>

       <select id="edgeProtection">
        <option value="off">Off</option>
        <option value="low" selected>Low</option>
        <option value="medium">Medium</option>
        <option value="high">High</option>
       </select>
      </div>
     </div>
    </details>
   </div>

   <div class="section">
    <button id="previewButton" class="secondary full" disabled>
     Generate High-Quality Preview
    </button>

    <button
     id="uploadBatchButton"
     class="primary full"
     style="margin-top:9px"
     disabled
    >
     Convert & Upload All
    </button>

    <div class="batch-actions">
     <button id="retryFailed" class="secondary">Retry Failed</button>
     <button id="removeCompleted" class="secondary">Remove Completed</button>
     <button id="clearQueue" class="danger">Clear Queue</button>
    </div>
   </div>
  </aside>

  <main class="card preview-wrap">
   <div class="row between" style="margin-bottom:12px">
    <div>
     <h2 id="previewName">Preview</h2>
     <span class="small">
      600 × 400 · neutral-calibrated six-color E6
     </span>
    </div>

    <div class="tabs">
     <button id="originalTab" class="active">Original</button>
     <button id="resultTab">E-Paper</button>
    </div>
   </div>

   <div class="canvas-frame">
    <canvas
     id="sourceCanvas"
     class="preview"
     width="600"
     height="400"
    ></canvas>

    <canvas
     id="resultCanvas"
     class="preview hidden"
     width="600"
     height="400"
    ></canvas>

    <div id="emptyPreview" class="empty">
     Select photos to begin
    </div>
   </div>

   <div class="status">
    <strong id="statusTitle">Ready</strong>

    <div id="statusMessage" class="small">
     Select one or more photos.
    </div>

    <div class="progress">
     <div id="progressBar"></div>
    </div>
   </div>
  </main>
 </div>
</section>

<section id="libraryView" class="view">

 <div class="card slideshow-card">
  <div class="row between">
   <div>
    <h2>Slideshow</h2>
    <span id="selectedSummary" class="small">0 selected photos</span>
   </div>

   <span id="busyBadge" class="small"></span>
  </div>

  <div class="stats">
   <div class="stat">
    <span class="small">Status</span>
    <strong id="slideState">Paused</strong>
   </div>

   <div class="stat">
    <span class="small">Current</span>
    <strong id="currentPhoto">None</strong>
   </div>

   <div class="stat">
    <span class="small">Next change</span>
    <strong id="countdown">--</strong>
   </div>
  </div>

  <div class="row" style="margin-top:12px">
   <button id="previousSlide" class="secondary">Previous</button>
   <button id="toggleSlide" class="primary">Start</button>
   <button id="nextSlide" class="secondary">Next</button>
  </div>

  <details>
   <summary>Slideshow settings</summary>

   <div class="details-body">
    <div class="field">
     <label>Cycle interval in seconds</label>

     <input
      id="slideInterval"
      type="number"
      min="30"
      value="300"
     >

     <div class="warning">
      Minimum 30 seconds. Short intervals cause frequent full refreshes.
     </div>
    </div>

    <div class="row" style="margin-top:9px">
     <button class="secondary intervalPreset" data-seconds="30">30 sec</button>
     <button class="secondary intervalPreset" data-seconds="60">1 min</button>
     <button class="secondary intervalPreset" data-seconds="300">5 min</button>
     <button class="secondary intervalPreset" data-seconds="600">10 min</button>
    </div>

    <div class="field">
     <label>Playback order</label>

     <select id="slideMode">
      <option value="sequential">Sequential</option>
      <option value="shuffle">Shuffle without repeats</option>
     </select>
    </div>


    <div class="field">
     <label>Slideshow categories</label>

     <div
      id="slideshowCategoryChoices"
      class="slideshow-category-list">
     </div>

     <div class="small" style="margin-top:7px">
      Leave every category unchecked to use all enabled photos.
     </div>
    </div>

    <div class="field">
     <label>Category matching</label>

     <select id="categoryMatch">
      <option value="any">
       Match any selected category
      </option>

      <option value="all">
       Match all selected categories
      </option>
     </select>
    </div>

    <div
     id="slideshowPoolSummary"
     class="small"
     style="margin-top:8px">
    </div>

    <button
     id="saveSlideshow"
     class="primary full"
     style="margin-top:13px"
    >
     Save Slideshow Settings
    </button>
   </div>
  </details>
 </div>

 <div class="card" style="margin-top:18px">
  <div class="library-toolbar row between">
   <div>
    <h2>Photo Library</h2>
    <span id="storageText" class="small"></span>
   </div>

   <div class="row">
    <button id="browseMode" class="primary">Browse</button>
    <button id="selectMode" class="secondary">Select</button>
    <button id="reorderMode" class="secondary">Reorder</button>
   </div>
  </div>


  <div class="category-controls">
   <select id="categoryFilter">
    <option value="all">All photos</option>
    <option value="__uncategorized__">
     Uncategorized
    </option>
   </select>

   <button
    id="manageCategories"
    class="secondary">
    Manage Categories
   </button>
  </div>

  <div id="library" class="library"></div>
  <div id="reorderList" class="reorder-list hidden"></div>

  <div id="selectionBar" class="selection-bar">
   <span id="selectionCount">0 checked</span>

   <div class="row">
    <button id="selectAll" class="secondary">All</button>
    <button id="selectNone" class="secondary">None</button>
    <button id="invertSelection" class="secondary">Invert</button>
    <button
     id="assignSelectedCategories"
     class="secondary">
     Categories
    </button>
    <button id="applySelection" class="primary">Apply Slideshow</button>
    <button id="deleteSelected" class="danger">Delete Checked</button>
   </div>
  </div>
 </div>
</section>


<section id="extensionsView" class="view">

 <div class="extensions-grid">
  <article class="extension-card">
   <div class="extension-icon">◷ ☁</div>

   <h2>Clock & Weather</h2>

   <p class="muted">
    Digital and analog clocks with current weather
    and a three-day forecast.
   </p>

   <div class="small">
    ● Installed
   </div>

   <button
    id="openClockWeather"
    class="primary full"
    style="margin-top:13px">
    Open Dashboard
   </button>
  </article>

  <article class="extension-card">
    <div class="extension-icon">▦</div>

    <h2>Calendar</h2>

    <p class="muted">
     Agenda, week and month views from ICS calendars.
    </p>

    <div class="small">
     ● Installed
    </div>

    <button
     id="openCalendar"
     class="primary full"
     style="margin-top:13px">
     Open Dashboard
    </button>
   </article>

  <article class="extension-card">
   <div class="extension-icon">◫</div>

   <h2>Information Board</h2>

   <p class="muted">
    Custom messages, QR codes and announcements.
   </p>

   <div class="small">
    Coming later
   </div>
  </article>

  
 </div>

 <div
  id="clockWeatherDashboard"
  class="extension-dashboard hidden"
  style="margin-top:18px">

  <aside class="card">
   <div class="section">
    <div class="row between">
     <h2>Clock & Weather</h2>

     <button
      id="closeClockWeather"
      class="ghost">
      Close
     </button>
    </div>

    <div class="field">
     <label>Dashboard layout</label>

     <select id="cwLayout">
      <option value="digital">
       Digital clock
      </option>

      <option value="analog">
       Classic analog clock
      </option>

      <option value="analog-modern">
       Modern analog clock
      </option>

      <option value="digital-weather">
       Digital clock + weather
      </option>

      <option value="analog-weather">
       Analog clock + weather
      </option>

      <option value="weather">
       Weather dashboard
      </option>
     </select>
    </div>

    <div class="field">
     <label>Search any city or postal code</label>

     <div class="row">
      <input
       id="cwLocationSearch"
       type="text"
       value="Hong Kong"
       placeholder="Hong Kong, London, Tokyo..."
       style="flex:1">

      <button
       id="cwSearchLocation"
       class="secondary">
       Search
      </button>
     </div>
    </div>

    <div
     id="cwSearchStatus"
     class="small"
     style="margin-top:7px">
    </div>

    <div class="field">
     <label>Search results</label>

     <select id="cwSearchResults">
      <option value="">
       Search for a global location
      </option>
     </select>
    </div>

    <div class="field">
     <label>Selected location name</label>

     <input
      id="cwLocation"
      type="text"
      value="Hong Kong"
      placeholder="Hong Kong">
    </div>

    <div class="row">
     <div class="field" style="flex:1">
      <label>Latitude</label>

      <input
       id="cwLatitude"
       type="number"
       step="0.000001"
       value="22.319300">
     </div>

     <div class="field" style="flex:1">
      <label>Longitude</label>

      <input
       id="cwLongitude"
       type="number"
       step="0.000001"
       value="114.169400">
     </div>
    </div>

    <button
     id="cwUseLocation"
     class="secondary full"
     style="margin-top:9px">
     Try Browser Location
    </button>

    <div class="small" style="margin-top:7px">
     Browser location may be blocked because the ESP32 page uses
     HTTP. Global city search works without browser-location
     permission.
    </div>

    <div class="field">
     <label>Time zone</label>

     <input
      id="cwTimezone"
      type="text"
      value="Asia/Hong_Kong"
      readonly>

     <div class="small" style="margin-top:5px">
      Local UTC offset is detected automatically from the selected
      weather location.
     </div>
    </div>

    <label class="checkline">
     <input
      id="cwTwelveHour"
      type="checkbox"
      checked>

     <span>Use 12-hour clock</span>
    </label>

    <div class="row">
     <div class="field" style="flex:1">
      <label>Temperature</label>

      <select id="cwTemperatureUnit">
       <option value="fahrenheit">
        Fahrenheit
       </option>

       <option value="celsius">
        Celsius
       </option>
      </select>
     </div>

     <div class="field" style="flex:1">
      <label>Wind</label>

      <select id="cwWindUnit">
       <option value="mph">mph</option>
       <option value="kmh">km/h</option>
      </select>
     </div>
    </div>

    <div class="field">
     <label>Physical display update</label>

     <select id="cwInterval">
      <option value="180">3 minutes</option>
      <option value="300">5 minutes</option>
      <option value="600">10 minutes</option>
      <option value="900">15 minutes</option>
      <option value="1800">30 minutes</option>
      <option value="3600">60 minutes</option>
     </select>
    </div>

    <div class="field">
     <label>Weather data refresh</label>

     <select id="cwWeatherInterval">
      <option value="600">10 minutes</option>
      <option value="1800">30 minutes</option>
      <option value="3600">60 minutes</option>
     </select>
    </div>

    <button
     id="cwSave"
     class="primary full"
     style="margin-top:13px">
     Save Dashboard Settings
    </button>
   </div>

   <div class="section">
    <div class="row">
     <button id="cwDisplayNow" class="secondary">
      Display Now
     </button>

     <button id="cwActivate" class="primary">
      Activate
     </button>

     <button id="cwDeactivate" class="danger">
      Deactivate
     </button>
    </div>

    <div class="row" style="margin-top:9px">
     <button id="cwSyncTime" class="secondary">
      Sync Time
     </button>

     <button id="cwRefreshWeather" class="secondary">
      Refresh Weather
     </button>
    </div>

    <p class="muted">
     Weather data by Open-Meteo.
     The physical display minimum is three minutes.
    </p>
   </div>
  </aside>

  <main class="card extension-preview">
   <div class="row between">
    <div>
     <h2>Dashboard Preview</h2>

     <span class="small">
      600 × 400 · E6 six color
     </span>
    </div>

    <button
     id="cwRefreshPreview"
     class="secondary">
     Refresh Preview
    </button>
   </div>

   <div class="canvas-frame" style="margin-top:12px">
    <canvas
     id="cwPreviewCanvas"
     width="600"
     height="400"
     style="display:block;width:100%">
    </canvas>
   </div>

   <div class="extension-status">
    <div class="extension-status-box">
     <span class="small">Extension</span>
     <strong id="cwActiveStatus">Inactive</strong>
    </div>

    <div class="extension-status-box">
     <span class="small">Current time</span>
     <strong id="cwCurrentTime">--</strong>
    </div>

    <div class="extension-status-box">
     <span class="small">Weather</span>
     <strong id="cwWeatherStatus">--</strong>
    </div>
   </div>
  </main>
 </div>



 <div
  id="calendarDashboard"
  class="calendar-dashboard hidden"
  style="margin-top:18px">

  <aside class="card">

   <div class="section">
    <div class="row between">
     <h2>Calendar</h2>

     <button
      id="closeCalendar"
      class="ghost">
      Close
     </button>
    </div>

    <div class="field">
     <label>Dashboard layout</label>

     <select id="calLayout">
      <option value="agenda">
       Agenda
      </option>

      <option value="agenda-month">
       Agenda + Mini Month
      </option>

      <option value="week">
       Week Planner
      </option>

      <option value="month">
       Month View
      </option>

      <option value="clock-agenda">
       Clock + Agenda
      </option>

      <option value="clock-weather-agenda">
       Clock + Weather + Agenda
      </option>
     </select>
    </div>

    <div class="field">
     <label>Event range</label>

     <select id="calRange">
      <option value="1">Today</option>
      <option value="2">Today + Tomorrow</option>
      <option value="3">Next 3 Days</option>
      <option value="7">Next 7 Days</option>
     </select>
    </div>

    <div class="field">
     <label>Maximum events</label>

     <input
      id="calMaximumEvents"
      type="number"
      min="1"
      max="10"
      value="6">
    </div>

    <div class="field">
     <label>Privacy</label>

     <select id="calPrivacy">
      <option value="full">
       Show event names and locations
      </option>

      <option value="busy">
       Show time and Busy
      </option>

      <option value="time-only">
       Show time only
      </option>
     </select>
    </div>

    <div class="field">
     <label>UTC offset in minutes</label>

     <input
      id="calOffset"
      type="number"
      min="-720"
      max="840"
      value="480">

     <div class="small" style="margin-top:5px">
      Hong Kong is 480.
     </div>
    </div>

    <div class="field">
     <label>Synchronization</label>

     <select id="calSyncInterval">
      <option value="300">5 minutes</option>
      <option value="900">15 minutes</option>
      <option value="1800">30 minutes</option>
      <option value="3600">60 minutes</option>
     </select>
    </div>

    <button
     id="calSaveSettings"
     class="primary full"
     style="margin-top:13px">
     Save Calendar Settings
    </button>
   </div>

   <div class="section">
    <h3>Calendar Feeds</h3>

    <p class="muted">
     Add an ICS URL. For Google Calendar use the
     Secret address in iCal format. Saved URLs are masked.
    </p>

    <div id="calendarFeeds"></div>
   </div>

   <div class="section">
    <div class="row">
     <button
      id="calDisplayNow"
      class="secondary">
      Display Now
     </button>

     <button
      id="calActivate"
      class="primary">
      Activate
     </button>

     <button
      id="calDeactivate"
      class="danger">
      Deactivate
     </button>

     <button
      id="calSyncNow"
      class="secondary">
      Sync Now
     </button>
    </div>
   </div>
  </aside>

  <main class="card calendar-preview">
   <div class="row between">
    <div>
     <h2>Calendar Preview</h2>

     <span class="small">
      Cached events remain available offline
     </span>
    </div>

    <button
     id="calRefreshPreview"
     class="secondary">
     Refresh Preview
    </button>
   </div>

   <div
    class="canvas-frame"
    style="margin-top:12px">

    <canvas
     id="calPreviewCanvas"
     width="600"
     height="400"
     style="display:block;width:100%">
    </canvas>
   </div>

   <div class="calendar-state">
    <div>
     <span class="small">Extension</span>
     <strong id="calActiveStatus">Inactive</strong>
    </div>

    <div>
     <span class="small">Cached events</span>
     <strong id="calEventCount">0</strong>
    </div>

    <div>
     <span class="small">Sync status</span>
     <strong id="calSyncStatus">--</strong>
    </div>
   </div>
  </main>
 </div>
</section>

<section id="settingsView" class="view">
 <div class="settings-grid">

  <div class="card">
   <div class="section">
    <h2>Wi-Fi and Network</h2>

    <div id="wifiStatus" class="muted"></div>

    <div class="field">
     <label>Available networks</label>

     <select id="networkList">
      <option value="">Scan networks…</option>
     </select>
    </div>

    <button id="scanNetworks" class="secondary full">
     Scan Wi-Fi
    </button>

    <div class="field">
     <label>Wi-Fi name</label>
     <input id="wifiSSID" type="text">
    </div>

    <div class="field">
     <label>Password</label>

     <input
      id="wifiPassword"
      type="password"
      placeholder="Blank keeps the existing password"
     >
    </div>

    <label class="checkline">
     <input id="openNetwork" type="checkbox">
     <span>This is an open network</span>
    </label>

    <button
     id="saveWiFi"
     class="primary full"
     style="margin-top:13px"
    >
     Save and Reconnect
    </button>

    <p class="muted">
     The permanent DynaPix-EPaper fallback access point remains active.
    </p>
   </div>
  </div>

  <div class="card">
   <div class="section">
    <h2>Device</h2>

    <div class="field">
     <label>Device name</label>
     <input id="deviceName" type="text">
    </div>

    <div class="field">
     <label>Hostname</label>
     <input id="hostname" type="text">

     <div class="small" style="margin-top:5px">
      http://<span id="hostnamePreview">dynapix</span>.local
     </div>
    </div>

    <div class="field">
     <label>Panel mounting</label>

     <select id="panelMount">
      <option value="left">Ribbon / connector on left</option>
      <option value="right">Ribbon / connector on right</option>
     </select>

     <div class="warning">
      Changing this affects new uploads. Existing images retain their
      original orientation.
     </div>
    </div>

    <button
     id="saveDevice"
     class="primary full"
     style="margin-top:13px"
    >
     Save Device Settings
    </button>
   </div>
  </div>

  <div class="card">
   <div class="section">
    <h2>Storage</h2>

    <div id="storageDetails" class="muted"></div>

    <div class="storage-bar">
     <div id="storageProgress"></div>
    </div>

    <button
     id="cleanStorage"
     class="secondary full"
     style="margin-top:13px"
    >
     Clean and Recover Storage
    </button>
   </div>
  </div>

  <div class="card">
   <div class="section">
    <h2>Maintenance</h2>

    <button id="restartDevice" class="secondary full">
     Restart ESP32
    </button>

    <button
     id="resetSlideshow"
     class="secondary full"
     style="margin-top:9px"
    >
     Reset Slideshow Settings
    </button>

    <button
     id="factoryReset"
     class="danger full"
     style="margin-top:20px"
    >
     Factory Reset
    </button>

    <p class="warning">
     Factory reset deletes all photos, Wi-Fi credentials and settings.
    </p>
   </div>
  </div>
 </div>
</section>
</div>

<div id="queueOverlay" class="overlay">
 <div class="modal">
  <div class="modal-header">
   <strong>Upload Queue</strong>
   <button class="ghost closeOverlay">Close</button>
  </div>

  <div id="queueList" class="modal-body queue-list"></div>
 </div>
</div>

<div id="menuOverlay" class="overlay">
 <div class="modal">
  <div class="modal-header">
   <strong id="menuPhotoName">Photo</strong>
   <button class="ghost closeOverlay">Close</button>
  </div>

  <div class="modal-body modal-actions">
   <button id="menuDisplay" class="primary">Display Now</button>

   <button id="menuToggleSelection" class="secondary">
    Include / Exclude from Slideshow
   </button>

   <button id="menuCategories" class="secondary">Categories</button>
   <button id="menuRename" class="secondary">Rename</button>
   <button id="menuMoveUp" class="secondary">Move Earlier</button>
   <button id="menuMoveDown" class="secondary">Move Later</button>
   <button id="menuDelete" class="danger">Delete</button>
  </div>
 </div>
</div>

<div id="detailOverlay" class="overlay">
 <div class="modal">
  <div class="modal-header">
   <strong id="detailName">Photo</strong>
   <button class="ghost closeOverlay">Close</button>
  </div>

  <div class="modal-body">
   <div class="canvas-frame">
    <canvas
     id="detailCanvas"
     width="600"
     height="400"
     style="width:100%;display:block"
    ></canvas>
   </div>

   <p id="detailMeta" class="muted"></p>

   <button id="detailDisplay" class="primary full">
    Display Now
   </button>
  </div>
 </div>
</div>


<div id="categoryManagerOverlay" class="overlay">
 <div class="modal">
  <div class="modal-header">
   <strong>Manage Categories</strong>
   <button class="ghost closeOverlay">Close</button>
  </div>

  <div class="modal-body">
   <div class="field">
    <label>New category name</label>

    <input
     id="newCategoryName"
     type="text"
     maxlength="32"
     placeholder="Family, Travel, Artwork...">
   </div>

   <div class="field">
    <label>Color</label>

    <input
     id="newCategoryColor"
     type="color"
     value="#16808a">
   </div>

   <button
    id="createCategory"
    class="primary full"
    style="margin-top:12px">
    Create Category
   </button>

   <div
    id="categoryManagerList"
    class="category-list"
    style="margin-top:15px">
   </div>
  </div>
 </div>
</div>

<div id="categoryAssignOverlay" class="overlay">
 <div class="modal">
  <div class="modal-header">
   <strong id="categoryAssignTitle">
    Assign Categories
   </strong>

   <button class="ghost closeOverlay">Close</button>
  </div>

  <div class="modal-body">
   <div id="categoryOperationField" class="field">
    <label>Batch operation</label>

    <select id="categoryOperation">
     <option value="add">
      Add selected categories
     </option>

     <option value="remove">
      Remove selected categories
     </option>

     <option value="replace">
      Replace existing categories
     </option>
    </select>
   </div>

   <div
    id="categoryAssignChoices"
    class="category-options"
    style="margin-top:12px">
   </div>

   <button
    id="saveCategoryAssignment"
    class="primary full"
    style="margin-top:13px">
    Apply Categories
   </button>
  </div>
 </div>
</div>

<script>
"use strict";

const W = 600;
const H = 400;
const PW = 400;
const PH = 600;
const BYTES = 120000;

const $ = id => document.getElementById(id);

/*
  Neutral-calibrated E6 palette.

  White is intentionally neutral rather than cream.

  Yellow remains strong, but selection of yellow is guarded
  later so neutral and warm-light areas do not become yellow.
*/
const palette = [
 {
  name: "black",
  code: 0x0,
  rgb: [12, 13, 13],
  preview: [5, 6, 6]
 },
 {
  name: "white",
  code: 0x1,
  rgb: [248, 248, 248],
  preview: [255, 255, 255]
 },
 {
  name: "yellow",
  code: 0x2,
  rgb: [226, 196, 35],
  preview: [239, 211, 29]
 },
 {
  name: "red",
  code: 0x3,
  rgb: [189, 47, 43],
  preview: [208, 47, 47]
 },
 {
  name: "blue",
  code: 0x5,
  rgb: [45, 77, 145],
  preview: [43, 80, 166]
 },
 {
  name: "green",
  code: 0x6,
  rgb: [57, 126, 71],
  preview: [57, 140, 76]
 }
];

const colors = {};

for (const color of palette) {
 colors[color.code] = color.preview;
}

const sourceCanvas = $("sourceCanvas");
const resultCanvas = $("resultCanvas");

let sctx;
let rctx;

try {
 sctx = sourceCanvas.getContext("2d", {
  willReadFrequently: true,
  colorSpace: "srgb"
 });

 rctx = resultCanvas.getContext("2d", {
  willReadFrequently: true,
  colorSpace: "srgb"
 });
} catch (error) {
 sctx = sourceCanvas.getContext(
  "2d",
  {willReadFrequently: true}
 );

 rctx = resultCanvas.getContext(
  "2d",
  {willReadFrequently: true}
 );
}

let pending = [];
let pendingIndex = -1;
let currentImage = null;
let packed = null;

let library = [];
let processing = false;

let dragging = false;
let pointerX = 0;
let pointerY = 0;

let libraryMode = "browse";
let activePhotoId = "";

let slideshowDirty = false;
let slideshowSaving = false;
let displayBusy = false;

let settings = {
 mount: "left"
};

let categories = [];
let libraryFilter = "all";
let slideshowCategoryIds = "";
let categoryMatchValue = "any";

let categoryAssignmentPhotoIds = [];
let categoryAssignmentIsBatch = false;

/*
  Every preset has reduced yellow/color pressure compared with
  the previous version.

  Maximum Detail keeps strong dithering but uses restrained
  chroma matching, so detail no longer means extra yellow.
*/
const qualityPresets = {
 vivid: {
  brightness: 3,
  contrast: 9,
  saturation: 0,
  vibrance: 18,
  shadow: 9,
  highlight: 20,
  sharpness: 25,
  dither: 68,
  colorStyle: 55,
  vividStrength: 58,
  ditherMode: "smart",
  edgeProtection: "medium",
  autoOptimize: true,
  protectHighlights: true,
  enhanceColor: true,
  sharpenEnabled: true,
  skinProtection: true
 },

 natural: {
  brightness: 2,
  contrast: 6,
  saturation: -2,
  vibrance: 8,
  shadow: 6,
  highlight: 22,
  sharpness: 21,
  dither: 64,
  colorStyle: 36,
  vividStrength: 38,
  ditherMode: "smart",
  edgeProtection: "medium",
  autoOptimize: true,
  protectHighlights: true,
  enhanceColor: true,
  sharpenEnabled: true,
  skinProtection: true
 },

 portrait: {
  brightness: 3,
  contrast: 4,
  saturation: -4,
  vibrance: 6,
  shadow: 9,
  highlight: 26,
  sharpness: 17,
  dither: 54,
  colorStyle: 32,
  vividStrength: 30,
  ditherMode: "smart",
  edgeProtection: "high",
  autoOptimize: true,
  protectHighlights: true,
  enhanceColor: true,
  sharpenEnabled: true,
  skinProtection: true
 },

 graphics: {
  brightness: 2,
  contrast: 13,
  saturation: 4,
  vibrance: 10,
  shadow: 2,
  highlight: 10,
  sharpness: 34,
  dither: 0,
  colorStyle: 68,
  vividStrength: 65,
  ditherMode: "clean",
  edgeProtection: "high",
  autoOptimize: false,
  protectHighlights: false,
  enhanceColor: true,
  sharpenEnabled: true,
  skinProtection: false
 },

 detail: {
  brightness: 2,
  contrast: 8,
  saturation: 0,
  vibrance: 16,
  shadow: 8,
  highlight: 22,
  sharpness: 30,
  dither: 84,
  colorStyle: 44,
  vividStrength: 46,
  ditherMode: "smart",
  edgeProtection: "low",
  autoOptimize: true,
  protectHighlights: true,
  enhanceColor: true,
  sharpenEnabled: true,
  skinProtection: true
 }
};

// ============================================================
// BROWSER HELPERS
// ============================================================

function api(path, options = {}) {
 return fetch(path, options).then(async response => {
  const text = await response.text();

  if (!response.ok) {
   let message = text || `HTTP ${response.status}`;

   try {
    const parsed = JSON.parse(text);
    message = parsed.error || parsed.message || message;
   } catch (error) {}

   throw Error(message);
  }

  try {
   return JSON.parse(text);
  } catch (error) {
   return text;
  }
 });
}

function form(data) {
 return {
  method: "POST",
  headers: {
   "Content-Type": "application/x-www-form-urlencoded"
  },
  body: new URLSearchParams(data)
 };
}

function escapeHtml(value) {
 return String(value).replace(
  /[&<>"']/g,
  character => ({
   "&": "&amp;",
   "<": "&lt;",
   ">": "&gt;",
   "\"": "&quot;",
   "'": "&#39;"
  })[character]
 );
}

function signed(value) {
 value = Number(value);

 return value > 0
  ? "+" + value
  : String(value);
}

function extension(file) {
 return file.name
  .split(".")
  .pop()
  .toLowerCase();
}

function supported(file) {
 return (
  file.type.startsWith("image/") ||
  [
   "heic",
   "heif",
   "jpg",
   "jpeg",
   "png",
   "webp",
   "bmp",
   "avif"
  ].includes(extension(file))
 );
}

function pause(milliseconds = 10) {
 return new Promise(resolve =>
  requestAnimationFrame(() =>
   setTimeout(resolve, milliseconds)
  )
 );
}

function clamp(value, minimum, maximum) {
 return Math.max(
  minimum,
  Math.min(maximum, value)
 );
}

function setStatus(title, message, progress) {
 $("statusTitle").textContent = title;
 $("statusMessage").textContent = message;

 if (progress !== undefined) {
  $("progressBar").style.width =
   clamp(progress, 0, 100) + "%";
 }
}

function openOverlay(id) {
 $(id).classList.add("visible");
}

function closeOverlays() {
 document
  .querySelectorAll(".overlay")
  .forEach(item =>
   item.classList.remove("visible")
  );
}

document
 .querySelectorAll(".closeOverlay")
 .forEach(button => {
  button.onclick = closeOverlays;
 });

document
 .querySelectorAll(".overlay")
 .forEach(overlay => {
  overlay.onclick = event => {
   if (event.target === overlay) {
    closeOverlays();
   }
  };
 });

document
 .querySelectorAll(".nav")
 .forEach(button => {
  button.onclick = () => {
   document
    .querySelectorAll(".nav,.view")
    .forEach(item =>
     item.classList.remove("active")
    );

   button.classList.add("active");
   $(button.dataset.view).classList.add("active");

   if (button.dataset.view === "libraryView") {
    loadLibrary();
   }

   if (
    button.dataset.view ===
    "extensionsView"
   ) {
    loadClockWeather().catch(
     error => console.error(error)
    );

    loadCalendar().catch(
     error => console.error(error)
    );
   }
  };
 });

// ============================================================
// QUALITY PRESETS
// ============================================================

function updateQualityLabels() {
 const signedControls = [
  "brightness",
  "contrast",
  "saturation",
  "vibrance",
  "shadowDetail"
 ];

 for (const id of signedControls) {
  $(id + "Value").textContent =
   signed($(id).value);
 }

 const normalControls = [
  "highlightDetail",
  "sharpness",
  "ditherStrength",
  "colorStyle",
  "vividStrength"
 ];

 for (const id of normalControls) {
  $(id + "Value").textContent =
   $(id).value;
 }
}

function applyQualityPreset(name) {
 const preset =
  qualityPresets[name] ||
  qualityPresets.detail;

 $("brightness").value = preset.brightness;
 $("contrast").value = preset.contrast;
 $("saturation").value = preset.saturation;
 $("vibrance").value = preset.vibrance;
 $("shadowDetail").value = preset.shadow;
 $("highlightDetail").value = preset.highlight;
 $("sharpness").value = preset.sharpness;
 $("ditherStrength").value = preset.dither;
 $("colorStyle").value = preset.colorStyle;
 $("vividStrength").value = preset.vividStrength;

 $("ditherMode").value = preset.ditherMode;
 $("edgeProtection").value = preset.edgeProtection;

 $("autoOptimize").checked = preset.autoOptimize;
 $("protectHighlights").checked = preset.protectHighlights;
 $("enhanceColor").checked = preset.enhanceColor;
 $("sharpenEnabled").checked = preset.sharpenEnabled;
 $("skinProtection").checked = preset.skinProtection;

 updateQualityLabels();
 renderSource();
}

// ============================================================
// COLOR SCIENCE
// ============================================================

function srgbChannelToLinear(value) {
 value = clamp(value / 255, 0, 1);

 return value <= 0.04045
  ? value / 12.92
  : Math.pow(
     (value + 0.055) / 1.055,
     2.4
    );
}

function rgbToOklab(red, green, blue) {
 const r = srgbChannelToLinear(red);
 const g = srgbChannelToLinear(green);
 const b = srgbChannelToLinear(blue);

 const l =
  0.4122214708 * r +
  0.5363325363 * g +
  0.0514459929 * b;

 const m =
  0.2119034982 * r +
  0.6806995451 * g +
  0.1073969566 * b;

 const s =
  0.0883024619 * r +
  0.2817188376 * g +
  0.6299787005 * b;

 const lRoot = Math.cbrt(Math.max(0, l));
 const mRoot = Math.cbrt(Math.max(0, m));
 const sRoot = Math.cbrt(Math.max(0, s));

 return {
  L:
   0.2104542553 * lRoot +
   0.7936177850 * mRoot -
   0.0040720468 * sRoot,

  a:
   1.9779984951 * lRoot -
   2.4285922050 * mRoot +
   0.4505937099 * sRoot,

  b:
   0.0259040371 * lRoot +
   0.7827717662 * mRoot -
   0.8086757660 * sRoot
 };
}

for (const color of palette) {
 color.lab = rgbToOklab(
  color.rgb[0],
  color.rgb[1],
  color.rgb[2]
 );

 color.chroma = Math.hypot(
  color.lab.a,
  color.lab.b
 );
}

function luminance(red, green, blue) {
 return (
  red * 0.2126 +
  green * 0.7152 +
  blue * 0.0722
 );
}

function isSkinTone(red, green, blue) {
 const maximum = Math.max(red, green, blue);
 const minimum = Math.min(red, green, blue);

 return (
  red > 70 &&
  green > 35 &&
  blue > 20 &&
  red > green &&
  green >= blue * 0.72 &&
  maximum - minimum > 12 &&
  red - green < 105
 );
}

function calculateHistogramLevels(data) {
 const histogram = new Uint32Array(256);
 let total = 0;

 for (let pixel = 0; pixel < W * H; pixel++) {
  const index = pixel * 4;

  const value = Math.round(
   luminance(
    data[index],
    data[index + 1],
    data[index + 2]
   )
  );

  histogram[
   clamp(value, 0, 255)
  ]++;

  total++;
 }

 const lowTarget = total * 0.005;
 const highTarget = total * 0.995;

 let cumulative = 0;
 let low = 0;
 let high = 255;

 for (let index = 0; index < 256; index++) {
  cumulative += histogram[index];

  if (cumulative >= lowTarget) {
   low = index;
   break;
  }
 }

 cumulative = 0;

 for (let index = 0; index < 256; index++) {
  cumulative += histogram[index];

  if (cumulative >= highTarget) {
   high = index;
   break;
  }
 }

 if (high - low < 80) {
  const center = (high + low) / 2;

  low = clamp(center - 40, 0, 255);
  high = clamp(center + 40, 0, 255);
 }

 return {low, high};
}

/*
  Gentle white balance.

  Only 25% of the estimated gray-world correction is used.
  This avoids overcorrecting warm indoor, skin-heavy, wood,
  sunset, cream, and beige photographs.
*/
function calculateWhiteBalance(data) {
 let redTotal = 0;
 let greenTotal = 0;
 let blueTotal = 0;
 let count = 0;

 for (
  let pixel = 0;
  pixel < W * H;
  pixel += 4
 ) {
  const index = pixel * 4;

  const red = data[index];
  const green = data[index + 1];
  const blue = data[index + 2];

  const light = luminance(red, green, blue);
  const maximum = Math.max(red, green, blue);
  const minimum = Math.min(red, green, blue);

  if (
   light > 50 &&
   light < 215 &&
   maximum - minimum < 65
  ) {
   redTotal += red;
   greenTotal += green;
   blueTotal += blue;
   count++;
  }
 }

 if (count < 150) {
  return {
   red: 1,
   green: 1,
   blue: 1
  };
 }

 const redAverage = redTotal / count;
 const greenAverage = greenTotal / count;
 const blueAverage = blueTotal / count;

 const target =
  (
   redAverage +
   greenAverage +
   blueAverage
  ) / 3;

 const estimatedRed = clamp(
  target / Math.max(1, redAverage),
  0.92,
  1.08
 );

 const estimatedGreen = clamp(
  target / Math.max(1, greenAverage),
  0.95,
  1.05
 );

 const estimatedBlue = clamp(
  target / Math.max(1, blueAverage),
  0.92,
  1.08
 );

 const correctionStrength = 0.25;

 return {
  red:
   1 +
   (estimatedRed - 1) *
   correctionStrength,

  green:
   1 +
   (estimatedGreen - 1) *
   correctionStrength,

  blue:
   1 +
   (estimatedBlue - 1) *
   correctionStrength
 };
}

function applyVibrance(
 red,
 green,
 blue,
 amount,
 protectSkin
) {
 const maximum = Math.max(red, green, blue);
 const minimum = Math.min(red, green, blue);

 const chroma = maximum - minimum;

 const saturation =
  maximum > 0
   ? chroma / maximum
   : 0;

 let strength =
  amount *
  (1 - saturation * 0.72);

 if (
  protectSkin &&
  isSkinTone(red, green, blue)
 ) {
  strength *= 0.26;
 }

 const light = luminance(red, green, blue);

 return [
  light + (red - light) * (1 + strength),
  light + (green - light) * (1 + strength),
  light + (blue - light) * (1 + strength)
 ];
}

// ============================================================
// IMAGE PREPROCESSING
// ============================================================

function preprocessImage(sourceData) {
 const output =
  new Float32Array(W * H * 3);

 const autoOptimize =
  $("autoOptimize").checked;

 const protectHighlights =
  $("protectHighlights").checked;

 const enhanceColor =
  $("enhanceColor").checked;

 const protectSkin =
  $("skinProtection").checked;

 const vividStrength =
  Number($("vividStrength").value) / 100;

 const brightness =
  Number($("brightness").value);

 const contrast =
  Number($("contrast").value);

 const saturation =
  Number($("saturation").value) / 100;

 const vibrance =
  Number($("vibrance").value) / 100;

 const shadow =
  Number($("shadowDetail").value) / 100;

 const highlight =
  Number($("highlightDetail").value) / 100;

 const contrastFactor =
  (
   259 *
   (contrast + 255)
  ) /
  (
   255 *
   (259 - contrast)
  );

 const levels = autoOptimize
  ? calculateHistogramLevels(sourceData.data)
  : {low: 0, high: 255};

 const whiteBalance = autoOptimize
  ? calculateWhiteBalance(sourceData.data)
  : {red: 1, green: 1, blue: 1};

 const levelRange =
  Math.max(
   1,
   levels.high - levels.low
  );

 for (let pixel = 0; pixel < W * H; pixel++) {
  const sourceIndex = pixel * 4;
  const outputIndex = pixel * 3;

  let red =
   sourceData.data[sourceIndex] *
   whiteBalance.red;

  let green =
   sourceData.data[sourceIndex + 1] *
   whiteBalance.green;

  let blue =
   sourceData.data[sourceIndex + 2] *
   whiteBalance.blue;

  red += brightness * 1.25;
  green += brightness * 1.25;
  blue += brightness * 1.25;

  red =
   contrastFactor *
   (red - 128) +
   128;

  green =
   contrastFactor *
   (green - 128) +
   128;

  blue =
   contrastFactor *
   (blue - 128) +
   128;

  const neutralLight =
   luminance(red, green, blue);

  red =
   neutralLight +
   (red - neutralLight) *
   (1 + saturation);

  green =
   neutralLight +
   (green - neutralLight) *
   (1 + saturation);

  blue =
   neutralLight +
   (blue - neutralLight) *
   (1 + saturation);

  const oldLight =
   Math.max(
    0.001,
    luminance(red, green, blue)
   );

  let normalizedLight =
   (
    oldLight -
    levels.low
   ) / levelRange;

  normalizedLight =
   clamp(normalizedLight, 0, 1);

  if (autoOptimize) {
   normalizedLight +=
    shadow *
    Math.pow(
     1 - normalizedLight,
     2
    ) *
    0.42;

   normalizedLight +=
    vividStrength *
    (
     normalizedLight - 0.5
    ) *
    (
     1 -
     Math.abs(
      normalizedLight * 2 - 1
     )
    ) *
    0.10;

   if (protectHighlights) {
    const highlightPosition =
     clamp(
      (
       normalizedLight - 0.66
      ) / 0.34,
      0,
      1
     );

    normalizedLight -=
     highlight *
     highlightPosition *
     highlightPosition *
     0.18;
   }
  }

  normalizedLight =
   clamp(normalizedLight, 0, 1);

  const targetLight =
   normalizedLight * 255;

  const scale =
   targetLight / oldLight;

  red *= scale;
  green *= scale;
  blue *= scale;

  if (enhanceColor) {
   const vibrant = applyVibrance(
    red,
    green,
    blue,
    vibrance *
    (
     0.58 +
     vividStrength * 0.62
    ),
    protectSkin
   );

   red = vibrant[0];
   green = vibrant[1];
   blue = vibrant[2];
  }

  const currentLight =
   luminance(red, green, blue);

  const currentMaximum =
   Math.max(red, green, blue);

  const currentMinimum =
   Math.min(red, green, blue);

  const currentChroma =
   currentMaximum - currentMinimum;

  /*
    Neutral bright pixels are moved toward neutral white.
    This specifically reduces yellowish white backgrounds.
  */
  if (
   currentLight > 210 &&
   currentChroma < 45
  ) {
   const whiten =
    clamp(
     (
      currentLight - 210
     ) / 45,
     0,
     1
    ) *
    (
     0.45 +
     vividStrength * 0.20
    );

   red += (255 - red) * whiten;
   green += (255 - green) * whiten;
   blue += (255 - blue) * whiten;
  }

  output[outputIndex] =
   clamp(red, 0, 255);

  output[outputIndex + 1] =
   clamp(green, 0, 255);

  output[outputIndex + 2] =
   clamp(blue, 0, 255);
 }

 return output;
}

function sharpenImage(input) {
 if (
  !$("sharpenEnabled").checked ||
  Number($("sharpness").value) <= 0
 ) {
  return input;
 }

 const amount =
  Number($("sharpness").value) /
  100;

 const output =
  new Float32Array(input.length);

 for (let y = 0; y < H; y++) {
  for (let x = 0; x < W; x++) {
   const index =
    (y * W + x) * 3;

   for (
    let channel = 0;
    channel < 3;
    channel++
   ) {
    let total = 0;
    let samples = 0;

    for (
     let offsetY = -1;
     offsetY <= 1;
     offsetY++
    ) {
     const sampleY =
      clamp(
       y + offsetY,
       0,
       H - 1
      );

     for (
      let offsetX = -1;
      offsetX <= 1;
      offsetX++
     ) {
      const sampleX =
       clamp(
        x + offsetX,
        0,
        W - 1
       );

      total += input[
       (
        sampleY * W +
        sampleX
       ) * 3 +
       channel
      ];

      samples++;
     }
    }

    const blurred =
     total / samples;

    const original =
     input[index + channel];

    output[index + channel] =
     clamp(
      original +
      (
       original - blurred
      ) *
      amount *
      1.35,
      0,
      255
     );
   }
  }
 }

 return output;
}

function createEdgeMap(input) {
 const output =
  new Float32Array(W * H);

 for (let y = 1; y < H - 1; y++) {
  for (let x = 1; x < W - 1; x++) {
   const left =
    (y * W + x - 1) * 3;

   const right =
    (y * W + x + 1) * 3;

   const top =
    ((y - 1) * W + x) * 3;

   const bottom =
    ((y + 1) * W + x) * 3;

   const horizontal =
    luminance(
     input[right],
     input[right + 1],
     input[right + 2]
    ) -
    luminance(
     input[left],
     input[left + 1],
     input[left + 2]
    );

   const vertical =
    luminance(
     input[bottom],
     input[bottom + 1],
     input[bottom + 2]
    ) -
    luminance(
     input[top],
     input[top + 1],
     input[top + 2]
    );

   output[y * W + x] =
    clamp(
     Math.hypot(
      horizontal,
      vertical
     ) / 120,
     0,
     1
    );
  }
 }

 return output;
}

// ============================================================
// PALETTE MATCHING WITH YELLOW PROTECTION
// ============================================================

function nearestPaletteColor(
 red,
 green,
 blue
) {
 const sourceLab =
  rgbToOklab(red, green, blue);

 const sourceChroma =
  Math.hypot(
   sourceLab.a,
   sourceLab.b
  );

 const style =
  Number($("colorStyle").value) /
  100;

 const vivid =
  Number($("vividStrength").value) /
  100;

 const light =
  luminance(red, green, blue) /
  255;

 const skin =
  isSkinTone(red, green, blue);

 let best = palette[0];
 let bestDistance = Infinity;

 for (const color of palette) {
  const lightDifference =
   sourceLab.L - color.lab.L;

  const aDifference =
   sourceLab.a - color.lab.a;

  const bDifference =
   sourceLab.b - color.lab.b;

  let distance =
   lightDifference *
   lightDifference *
   (2.45 - style * 0.35) +

   aDifference *
   aDifference *
   (1.10 + style * 0.14) +

   bDifference *
   bDifference *
   (1.10 + style * 0.14);

  /*
    Prevent excessive black in midtones.
  */
  if (
   color.code === 0x0 &&
   light > 0.20
  ) {
   distance +=
    Math.pow(light - 0.20, 2) *
    (
     0.07 +
     style * 0.04
    );
  }

  /*
    Favor neutral white in bright regions.
  */
  if (
   color.code === 0x1 &&
   light > 0.70
  ) {
   distance -=
    (
     light - 0.70
    ) *
    (
     0.025 +
     style * 0.012
    );
  }

  /*
    Strong neutral protection.

    White, gray, cream, pale beige and near-neutral pixels
    must not receive colored ink unless there is meaningful
    source chroma.
  */
  if (
   sourceChroma < 0.065 &&
   color.chroma > 0.04
  ) {
   const neutralAmount =
    (
     0.065 -
     sourceChroma
    ) / 0.065;

   distance +=
    neutralAmount *
    (
     0.040 +
     style * 0.018
    );
  }

  /*
    Yellow guard.

    Yellow is allowed only when the source has genuine yellow
    chroma. Warm neutral, cream, beige, skin highlights and
    slightly warm whites receive a significant penalty.
  */
  if (color.code === 0x2) {
   const genuinelyYellow =
    sourceChroma > 0.078 &&
    sourceLab.b > 0.060 &&
    sourceLab.b >
     Math.abs(sourceLab.a) * 0.72;

   if (!genuinelyYellow) {
    const neutralPenalty =
     clamp(
      (
       0.078 -
       sourceChroma
      ) / 0.078,
      0,
      1
     );

    distance +=
     0.040 +
     neutralPenalty * 0.070;
   }

   /*
     Skin tones may use a controlled amount of yellow,
     but solid yellow should be strongly discouraged.
   */
   if (skin) {
    distance += 0.034;
   }

   /*
     Bright low-chroma pixels should nearly always be white.
   */
   if (
    light > 0.72 &&
    sourceChroma < 0.10
   ) {
    distance +=
     (
      light - 0.72
     ) *
     0.10;
   }
  }

  /*
    Encourage colored ink only when the hue genuinely agrees
    with the physical ink hue.
  */
  if (
   color.chroma > 0.04 &&
   sourceChroma > 0.045
  ) {
   const sourceLength =
    Math.max(
     0.0001,
     sourceChroma
    );

   const colorLength =
    Math.max(
     0.0001,
     color.chroma
    );

   const hueAgreement =
    (
     sourceLab.a * color.lab.a +
     sourceLab.b * color.lab.b
    ) /
    (
     sourceLength *
     colorLength
    );

   if (hueAgreement > 0.25) {
    distance -=
     (
      hueAgreement - 0.25
     ) *
     sourceChroma *
     style *
     vivid *
     0.038;
   }
  }

  if (distance < bestDistance) {
   bestDistance = distance;
   best = color;
  }
 }

 return best;
}

function edgeProtectionFactor(edge) {
 const mode =
  $("edgeProtection").value;

 if (mode === "off") {
  return 1;
 }

 const strength =
  mode === "high"
   ? 0.78
   : mode === "medium"
    ? 0.58
    : 0.34;

 return clamp(
  1 - edge * strength,
  0.18,
  1
 );
}

// ============================================================
// ADD PHOTOS
// ============================================================

$("dropZone").onclick =
 () => $("fileInput").click();

$("fileInput").onchange = event => {
 addFiles([...event.target.files]);
 event.target.value = "";
};

$("dropZone").ondragover = event => {
 event.preventDefault();
 $("dropZone").classList.add("drag");
};

$("dropZone").ondragleave = () => {
 $("dropZone").classList.remove("drag");
};

$("dropZone").ondrop = event => {
 event.preventDefault();
 $("dropZone").classList.remove("drag");

 addFiles([
  ...event.dataTransfer.files
 ]);
};

function addFiles(files) {
 files
  .filter(supported)
  .forEach(file => {
   pending.push({
    file,
    name: file.name,
    rotation: 0,
    zoom: 100,
    panX: 0,
    panY: 0,
    state: "Waiting",
    error: ""
   });
  });

 renderPending();

 if (
  pendingIndex < 0 &&
  pending.length
 ) {
  showPending(0);
 }
}

function renderPending() {
 $("pendingStrip").innerHTML =
  pending.map((item, index) => `
   <button
    class="pending-chip ${
     index === pendingIndex
      ? "active"
      : ""
    }"
    onclick="showPending(${index})"
   >
    <strong>${escapeHtml(item.name)}</strong>
    <span>${escapeHtml(item.state)}</span>
   </button>
  `).join("");

 $("queuePosition").textContent =
  pending.length
   ? `${pendingIndex + 1} / ${pending.length}`
   : "0";

 $("queueList").innerHTML =
  pending.length
   ? pending.map((item, index) => `
      <div
       class="queue-row"
       title="${escapeHtml(item.error || "")}"
      >
       <span>
        ${index + 1}. ${escapeHtml(item.name)}
       </span>

       <span>${escapeHtml(item.state)}</span>
      </div>
     `).join("")
   : '<div class="muted">Queue is empty.</div>';

 $("uploadBatchButton").disabled =
  !pending.length ||
  processing;

 $("previewButton").disabled =
  !pending.length ||
  processing;
}

$("openQueue").onclick = () => {
 renderPending();
 openOverlay("queueOverlay");
};

async function decodeImage(file) {
 try {
  return await createImageBitmap(
   file,
   {imageOrientation: "from-image"}
  );
 } catch (error) {
  return await new Promise(
   (resolve, reject) => {
    const url =
     URL.createObjectURL(file);

    const image =
     new Image();

    image.onload = () => {
     URL.revokeObjectURL(url);
     resolve(image);
    };

    image.onerror = () => {
     URL.revokeObjectURL(url);

     if (
      ["heic", "heif"].includes(
       extension(file)
      )
     ) {
      reject(Error(
       "HEIC could not be decoded by this browser. " +
       "Use current Safari on iPhone/iPad or share as JPEG."
      ));
     } else {
      reject(Error(
       "Image could not be decoded."
      ));
     }
    };

    image.src = url;
   }
  );
 }
}

window.showPending = async function(index) {
 if (
  index < 0 ||
  index >= pending.length
 ) {
  return;
 }

 pendingIndex = index;
 currentImage = null;
 packed = null;

 const item =
  pending[index];

 $("zoom").value =
  item.zoom;

 $("zoomValue").textContent =
  item.zoom + "%";

 $("previewName").textContent =
  item.name;

 item.state = "Loading";
 renderPending();

 setStatus(
  "Loading photo",
  `${index + 1} of ${pending.length}: ${item.name}`,
  5
 );

 try {
  currentImage =
   await decodeImage(item.file);

  item.state = "Ready";
  item.error = "";

  $("emptyPreview").classList.add("hidden");

  renderSource();
  showOriginal();
  renderPending();

  setStatus(
   "Photo ready",
   "Drag to position, zoom, rotate, or generate the preview.",
   15
  );
 } catch (error) {
  currentImage = null;
  item.state = "Decode failed";
  item.error = error.message;

  renderPending();

  setStatus(
   "Unable to load photo",
   `${item.name}: ${error.message}`,
   0
  );

  throw error;
 }
};

function renderSource() {
 sctx.setTransform(
  1, 0, 0, 1, 0, 0
 );

 sctx.filter = "none";
 sctx.fillStyle = "#ffffff";
 sctx.fillRect(0, 0, W, H);

 if (
  !currentImage ||
  pendingIndex < 0
 ) {
  return;
 }

 const item =
  pending[pendingIndex];

 const imageWidth =
  currentImage.width;

 const imageHeight =
  currentImage.height;

 const rotated =
  Math.abs(
   item.rotation % 180
  ) === 90;

 const rotatedWidth =
  rotated
   ? imageHeight
   : imageWidth;

 const rotatedHeight =
  rotated
   ? imageWidth
   : imageHeight;

 let scaleX;
 let scaleY;

 if ($("fitMode").value === "stretch") {
  scaleX =
   W / rotatedWidth;

  scaleY =
   H / rotatedHeight;
 } else {
  const base =
   $("fitMode").value === "contain"
    ? Math.min(
       W / rotatedWidth,
       H / rotatedHeight
      )
    : Math.max(
       W / rotatedWidth,
       H / rotatedHeight
      );

  scaleX = base;
  scaleY = base;
 }

 scaleX *= item.zoom / 100;
 scaleY *= item.zoom / 100;

 sctx.save();

 sctx.translate(
  W / 2 + item.panX,
  H / 2 + item.panY
 );

 sctx.rotate(
  item.rotation *
  Math.PI /
  180
 );

 sctx.scale(
  scaleX,
  scaleY
 );

 sctx.drawImage(
  currentImage,
  -imageWidth / 2,
  -imageHeight / 2
 );

 sctx.restore();
}

function packForPanel(codes, mount) {
 const output =
  new Uint8Array(BYTES);

 let outputPixel = 0;

 for (
  let panelY = 0;
  panelY < PH;
  panelY++
 ) {
  for (
   let panelX = 0;
   panelX < PW;
   panelX++
  ) {
   let sourceX;
   let sourceY;

   if (mount === "right") {
    sourceX = panelY;
    sourceY = H - 1 - panelX;
   } else {
    sourceX = W - 1 - panelY;
    sourceY = panelX;
   }

   const color =
    codes[
     sourceY * W +
     sourceX
    ] & 15;

   if (!(outputPixel & 1)) {
    output[outputPixel >> 1] =
     color << 4;
   } else {
    output[outputPixel >> 1] |=
     color;
   }

   outputPixel++;
  }
 }

 return output;
}

// ============================================================
// HIGH-QUALITY CONVERSION
// ============================================================

async function convertCurrent(
 showResultAfter = true
) {
 if (
  !currentImage ||
  pendingIndex < 0
 ) {
  throw Error(
   "No photo is selected."
  );
 }

 renderSource();
 await pause();

 setStatus(
  "Analyzing photo",
  "Optimizing brightness, neutral balance and color...",
  24
 );

 const sourceData =
  sctx.getImageData(
   0,
   0,
   W,
   H
  );

 let working =
  preprocessImage(sourceData);

 await pause();

 setStatus(
  "Enhancing detail",
  "Applying sharpening and edge protection...",
  36
 );

 working =
  sharpenImage(working);

 const edgeMap =
  createEdgeMap(working);

 const codes =
  new Uint8Array(W * H);

 const result =
  rctx.createImageData(W, H);

 const ditherMode =
  $("ditherMode").value;

 const baseDitherStrength =
  Number(
   $("ditherStrength").value
  ) / 100;

 const protectSkin =
  $("skinProtection").checked;

 function addError(
  x,
  y,
  redError,
  greenError,
  blueError,
  amount,
  currentEdge,
  skin
 ) {
  if (
   x < 0 ||
   x >= W ||
   y < 0 ||
   y >= H
  ) {
   return;
  }

  const index =
   (y * W + x) * 3;

  let strength =
   baseDitherStrength *
   edgeProtectionFactor(
    currentEdge
   );

  if (
   skin &&
   protectSkin
  ) {
   strength *= 0.62;
  }

  const clippedRed =
   clamp(redError, -78, 78);

  const clippedGreen =
   clamp(greenError, -78, 78);

  const clippedBlue =
   clamp(blueError, -78, 78);

  working[index] +=
   clippedRed *
   amount *
   strength;

  working[index + 1] +=
   clippedGreen *
   amount *
   strength;

  working[index + 2] +=
   clippedBlue *
   amount *
   strength;
 }

 setStatus(
  "Creating six-color image",
  "Using neutral-protected perceptual matching...",
  50
 );

 for (let y = 0; y < H; y++) {
  const reverse =
   ditherMode === "smart" &&
   (y & 1) === 1;

  const startX =
   reverse
    ? W - 1
    : 0;

  const endX =
   reverse
    ? -1
    : W;

  const direction =
   reverse
    ? -1
    : 1;

  for (
   let x = startX;
   x !== endX;
   x += direction
  ) {
   const pixel =
    y * W + x;

   const workingIndex =
    pixel * 3;

   const outputIndex =
    pixel * 4;

   const red =
    clamp(
     working[workingIndex],
     0,
     255
    );

   const green =
    clamp(
     working[workingIndex + 1],
     0,
     255
    );

   const blue =
    clamp(
     working[workingIndex + 2],
     0,
     255
    );

   const selected =
    nearestPaletteColor(
     red,
     green,
     blue
    );

   result.data[outputIndex] =
    selected.preview[0];

   result.data[outputIndex + 1] =
    selected.preview[1];

   result.data[outputIndex + 2] =
    selected.preview[2];

   result.data[outputIndex + 3] =
    255;

   codes[pixel] =
    selected.code;

   if (ditherMode !== "clean") {
    const selectedLight =
     luminance(
      selected.rgb[0],
      selected.rgb[1],
      selected.rgb[2]
     );

    const sourceLight =
     luminance(
      red,
      green,
      blue
     );

    const lightError =
     sourceLight -
     selectedLight;

    const skin =
     isSkinTone(
      red,
      green,
      blue
     );

    /*
      Chromatic error is deliberately lower than luminance
      error to avoid spreading yellow into neutral regions.
    */
    const colorErrorStrength =
     protectSkin && skin
      ? 0.40
      : 0.60;

    const rawRedError =
     red -
     selected.rgb[0];

    const rawGreenError =
     green -
     selected.rgb[1];

    const rawBlueError =
     blue -
     selected.rgb[2];

    const redError =
     lightError +
     (
      rawRedError -
      lightError
     ) *
     colorErrorStrength;

    const greenError =
     lightError +
     (
      rawGreenError -
      lightError
     ) *
     colorErrorStrength;

    const blueError =
     lightError +
     (
      rawBlueError -
      lightError
     ) *
     colorErrorStrength;

    const edge =
     edgeMap[pixel];

    if (ditherMode === "atkinson") {
     const directionValue = direction;

     addError(
      x + directionValue,
      y,
      redError,
      greenError,
      blueError,
      1 / 8,
      edge,
      skin
     );

     addError(
      x + directionValue * 2,
      y,
      redError,
      greenError,
      blueError,
      1 / 8,
      edge,
      skin
     );

     addError(
      x - directionValue,
      y + 1,
      redError,
      greenError,
      blueError,
      1 / 8,
      edge,
      skin
     );

     addError(
      x,
      y + 1,
      redError,
      greenError,
      blueError,
      1 / 8,
      edge,
      skin
     );

     addError(
      x + directionValue,
      y + 1,
      redError,
      greenError,
      blueError,
      1 / 8,
      edge,
      skin
     );

     addError(
      x,
      y + 2,
      redError,
      greenError,
      blueError,
      1 / 8,
      edge,
      skin
     );
    } else if (!reverse) {
     addError(
      x + 1,
      y,
      redError,
      greenError,
      blueError,
      7 / 16,
      edge,
      skin
     );

     addError(
      x - 1,
      y + 1,
      redError,
      greenError,
      blueError,
      3 / 16,
      edge,
      skin
     );

     addError(
      x,
      y + 1,
      redError,
      greenError,
      blueError,
      5 / 16,
      edge,
      skin
     );

     addError(
      x + 1,
      y + 1,
      redError,
      greenError,
      blueError,
      1 / 16,
      edge,
      skin
     );
    } else {
     addError(
      x - 1,
      y,
      redError,
      greenError,
      blueError,
      7 / 16,
      edge,
      skin
     );

     addError(
      x + 1,
      y + 1,
      redError,
      greenError,
      blueError,
      3 / 16,
      edge,
      skin
     );

     addError(
      x,
      y + 1,
      redError,
      greenError,
      blueError,
      5 / 16,
      edge,
      skin
     );

     addError(
      x - 1,
      y + 1,
      redError,
      greenError,
      blueError,
      1 / 16,
      edge,
      skin
     );
    }
   }
  }

  if (!(y % 40)) {
   setStatus(
    "Creating six-color image",
    `Processing row ${y + 1} of ${H}...`,
    50 + y / H * 12
   );

   await pause();
  }
 }

 rctx.putImageData(
  result,
  0,
  0
 );

 setStatus(
  "Packing display data",
  "Preparing native E-paper data...",
  63
 );

 await pause();

 packed =
  packForPanel(
   codes,
   settings.mount
  );

 if (showResultAfter) {
  showResult();
 }

 return packed;
}

function showOriginal() {
 $("originalTab").classList.add("active");
 $("resultTab").classList.remove("active");

 sourceCanvas.classList.remove("hidden");
 resultCanvas.classList.add("hidden");
}

function showResult() {
 $("resultTab").classList.add("active");
 $("originalTab").classList.remove("active");

 resultCanvas.classList.remove("hidden");
 sourceCanvas.classList.add("hidden");
}

$("originalTab").onclick = showOriginal;
$("resultTab").onclick = showResult;

sourceCanvas.onpointerdown = event => {
 if (
  pendingIndex < 0 ||
  processing
 ) {
  return;
 }

 dragging = true;
 pointerX = event.clientX;
 pointerY = event.clientY;

 sourceCanvas.setPointerCapture(
  event.pointerId
 );
};

sourceCanvas.onpointermove = event => {
 if (!dragging) {
  return;
 }

 const bounds =
  sourceCanvas.getBoundingClientRect();

 const item =
  pending[pendingIndex];

 item.panX +=
  (
   event.clientX -
   pointerX
  ) *
  W /
  bounds.width;

 item.panY +=
  (
   event.clientY -
   pointerY
  ) *
  H /
  bounds.height;

 pointerX = event.clientX;
 pointerY = event.clientY;

 renderSource();
};

sourceCanvas.onpointerup =
sourceCanvas.onpointercancel =
 () => dragging = false;

$("zoom").oninput = event => {
 if (pendingIndex < 0) {
  return;
 }

 pending[pendingIndex].zoom =
  Number(event.target.value);

 $("zoomValue").textContent =
  event.target.value + "%";

 renderSource();
};

$("rotateLeft").onclick = () => {
 if (pendingIndex < 0) {
  return;
 }

 pending[pendingIndex].rotation =
  (
   pending[pendingIndex].rotation +
   270
  ) % 360;

 renderSource();
};

$("rotateRight").onclick = () => {
 if (pendingIndex < 0) {
  return;
 }

 pending[pendingIndex].rotation =
  (
   pending[pendingIndex].rotation +
   90
  ) % 360;

 renderSource();
};

$("previousPending").onclick = () => {
 if (pending.length) {
  showPending(
   Math.max(
    0,
    pendingIndex - 1
   )
  );
 }
};

$("nextPending").onclick = () => {
 if (pending.length) {
  showPending(
   Math.min(
    pending.length - 1,
    pendingIndex + 1
   )
  );
 }
};

$("removePending").onclick = () => {
 if (pendingIndex < 0) {
  return;
 }

 pending.splice(
  pendingIndex,
  1
 );

 pendingIndex =
  Math.min(
   pendingIndex,
   pending.length - 1
  );

 renderPending();

 if (pendingIndex >= 0) {
  showPending(pendingIndex);
 } else {
  currentImage = null;

  $("emptyPreview")
   .classList
   .remove("hidden");
 }
};

$("qualityPreset").onchange = () => {
 applyQualityPreset(
  $("qualityPreset").value
 );
};

const qualityRangeControls = [
 "brightness",
 "contrast",
 "saturation",
 "vibrance",
 "shadowDetail",
 "highlightDetail",
 "sharpness",
 "ditherStrength",
 "colorStyle",
 "vividStrength"
];

for (const id of qualityRangeControls) {
 $(id).oninput = () => {
  updateQualityLabels();
 };
}

const qualityOptionControls = [
 "fitMode",
 "ditherMode",
 "edgeProtection",
 "autoOptimize",
 "protectHighlights",
 "enhanceColor",
 "sharpenEnabled",
 "skinProtection"
];

for (const id of qualityOptionControls) {
 $(id).onchange = () => {
  renderSource();
 };
}

$("previewButton").onclick = async () => {
 if (processing) {
  return;
 }

 processing = true;
 renderPending();

 setStatus(
  "Starting quality conversion",
  "Analyzing and neutralizing the photo...",
  18
 );

 try {
  await convertCurrent(true);

  setStatus(
   "High-quality preview ready",
   "The neutral-protected E-paper result is shown.",
   65
  );
 } catch (error) {
  setStatus(
   "Conversion failed",
   error.message,
   0
  );
 }

 processing = false;
 renderPending();
};

async function uploadOne(bytes, name) {
 if (
  !bytes ||
  bytes.length !== BYTES
 ) {
  throw Error(
   "Invalid converted image size."
  );
 }

 const data = new FormData();

 data.append(
  "image",
  new Blob(
   [bytes],
   {
    type: "application/octet-stream"
   }
  ),
  name + ".e6"
 );

 const response =
  await fetch(
   "/api/photos/upload" +
   "?name=" +
   encodeURIComponent(name) +
   "&mount=" +
   encodeURIComponent(settings.mount) +
   "&_=" +
   Date.now(),
   {
    method: "POST",
    body: data,
    cache: "no-store"
   }
  );

 const text =
  await response.text();

 if (!response.ok) {
  throw Error(
   text ||
   "Upload failed."
  );
 }

 return JSON.parse(text);
}

async function runBatch(indices) {
 if (
  processing ||
  !indices.length
 ) {
  return;
 }

 processing = true;
 renderPending();

 let uploaded = 0;
 let duplicates = 0;
 let failed = 0;

 for (
  let sequence = 0;
  sequence < indices.length;
  sequence++
 ) {
  const index =
   indices[sequence];

  const item =
   pending[index];

  try {
   await showPending(index);

   item.state = "Converting";
   item.error = "";

   renderPending();

   setStatus(
    `Converting ${sequence + 1} of ${indices.length}`,
    item.name,
    sequence /
    indices.length *
    90
   );

   const bytes =
    await convertCurrent(false);

   item.state = "Uploading";
   renderPending();

   const result =
    await uploadOne(
     bytes,
     item.name
    );

   item.state = "Uploaded";
   item.uploadedId =
    result.id || "";

   uploaded++;
  } catch (error) {
   item.error = error.message;

   if (
    error.message
     .toLowerCase()
     .includes("already exists")
   ) {
    item.state = "Already stored";
    duplicates++;
   } else {
    item.state =
     error.message
      .toLowerCase()
      .includes("decode")
      ? "Decode failed"
      : "Failed";

    failed++;
   }
  }

  renderPending();
  await pause(180);
 }

 processing = false;
 renderPending();

 setStatus(
  failed
   ? "Batch completed with errors"
   : "Batch complete",
  `${uploaded} uploaded, ${duplicates} already stored, ${failed} failed.`,
  100
 );
}

$("uploadBatchButton").onclick = () => {
 const indices =
  pending
   .map((item, index) => ({
    item,
    index
   }))
   .filter(entry =>
    ![
     "Uploaded",
     "Already stored"
    ].includes(entry.item.state)
   )
   .map(entry => entry.index);

 runBatch(indices);
};

$("retryFailed").onclick = () => {
 const indices =
  pending
   .map((item, index) => ({
    item,
    index
   }))
   .filter(entry =>
    [
     "Failed",
     "Decode failed"
    ].includes(entry.item.state)
   )
   .map(entry => entry.index);

 if (!indices.length) {
  alert(
   "There are no failed photos to retry."
  );

  return;
 }

 runBatch(indices);
};

$("removeCompleted").onclick = () => {
 pending =
  pending.filter(item =>
   ![
    "Uploaded",
    "Already stored"
   ].includes(item.state)
  );

 pendingIndex =
  pending.length
   ? 0
   : -1;

 renderPending();

 if (pending.length) {
  showPending(0);
 } else {
  currentImage = null;

  $("emptyPreview")
   .classList
   .remove("hidden");
 }
};

$("clearQueue").onclick = () => {
 if (!pending.length) {
  return;
 }

 if (!confirm(
  "Clear the complete upload queue?"
 )) {
  return;
 }

 pending = [];
 pendingIndex = -1;
 currentImage = null;
 packed = null;

 renderPending();

 $("emptyPreview")
  .classList
  .remove("hidden");

 setStatus(
  "Queue cleared",
  "Select photos to begin.",
  0
 );
};


// ============================================================
// CATEGORY UI
// ============================================================

function categoryById(id) {
 return categories.find(
  category => category.id === id
 );
}

function categoryIdsArray(csv) {
 return String(csv || "")
  .split(",")
  .map(value => value.trim())
  .filter(Boolean);
}

function photoCategoryNames(photo) {
 return categoryIdsArray(photo.categoryIds)
  .map(categoryById)
  .filter(Boolean);
}

function renderCategoryChips(photo) {
 const assigned = photoCategoryNames(photo);

 if (!assigned.length) {
  return `
   <div class="category-chips">
    <span class="category-chip">
     Uncategorized
    </span>
   </div>
  `;
 }

 return `
  <div class="category-chips">
   ${assigned.map(category => `
    <span class="category-chip">
     <span
      class="category-dot"
      style="background:${escapeHtml(category.color)}">
     </span>
     ${escapeHtml(category.name)}
    </span>
   `).join("")}
  </div>
 `;
}

function photoPassesLibraryFilter(photo) {
 if (libraryFilter === "all") {
  return true;
 }

 const ids =
  categoryIdsArray(photo.categoryIds);

 if (
  libraryFilter === "__uncategorized__"
 ) {
  return ids.length === 0;
 }

 return ids.includes(libraryFilter);
}

function photoMatchesSlideshowCategories(photo) {
 const requested =
  categoryIdsArray(slideshowCategoryIds);

 if (!requested.length) {
  return true;
 }

 const assigned =
  categoryIdsArray(photo.categoryIds);

 const matches =
  requested.map(id =>
   id === "__uncategorized__"
    ? assigned.length === 0
    : assigned.includes(id)
  );

 return categoryMatchValue === "all"
  ? matches.every(Boolean)
  : matches.some(Boolean);
}

function updateSlideshowPoolSummary() {
 const enabled =
  library.filter(photo => photo.selected);

 const eligible =
  enabled.filter(photo =>
   photoMatchesSlideshowCategories(photo)
  );

 $("slideshowPoolSummary").textContent =
  `${eligible.length} of ${enabled.length} enabled photos match this source.`;
}

function renderCategoryFilters() {
 const filter = $("categoryFilter");

 const uncategorizedCount =
  library.filter(photo =>
   categoryIdsArray(photo.categoryIds).length === 0
  ).length;

 filter.innerHTML = `
  <option value="all">
   All photos (${library.length})
  </option>

  <option value="__uncategorized__">
   Uncategorized (${uncategorizedCount})
  </option>

  ${categories.map(category => `
   <option value="${category.id}">
    ${escapeHtml(category.name)} (${category.count})
   </option>
  `).join("")}
 `;

 filter.value =
  [...filter.options].some(
   option => option.value === libraryFilter
  )
   ? libraryFilter
   : "all";
}

function renderSlideshowCategoryChoices() {
 const requested =
  categoryIdsArray(slideshowCategoryIds);

 const choices = [
  {
   id:"__uncategorized__",
   name:"Uncategorized",
   color:"#7b8794"
  },
  ...categories
 ];

 $("slideshowCategoryChoices").innerHTML =
  choices.map(category => `
   <label class="category-option">
    <input
     class="slideshow-category-check"
     type="checkbox"
     value="${category.id}"
     ${requested.includes(category.id) ? "checked" : ""}
    >

    <span
     class="category-dot"
     style="background:${escapeHtml(category.color)}">
    </span>

    <span>${escapeHtml(category.name)}</span>
   </label>
  `).join("");

 document
  .querySelectorAll(
   ".slideshow-category-check"
  )
  .forEach(input => {
   input.onchange = () => {
    slideshowDirty = true;

    slideshowCategoryIds = [
     ...document.querySelectorAll(
      ".slideshow-category-check:checked"
     )
    ].map(item => item.value).join(",");

    updateSlideshowPoolSummary();
   };
  });

 $("categoryMatch").value =
  categoryMatchValue;

 updateSlideshowPoolSummary();
}

function renderCategoryManager() {
 $("categoryManagerList").innerHTML =
  categories.length
   ? categories.map(category => `
      <div class="category-row">
       <input
        id="categoryColor-${category.id}"
        type="color"
        value="${escapeHtml(category.color)}"
       >

       <input
        id="categoryName-${category.id}"
        type="text"
        maxlength="32"
        value="${escapeHtml(category.name)}"
       >

       <button
        class="secondary"
        onclick="saveCategory('${category.id}')">
        Save
       </button>

       <button
        class="danger"
        onclick="deleteCategory('${category.id}')">
        Delete
       </button>
      </div>
     `).join("")
   : '<div class="muted">No custom categories yet.</div>';
}

window.saveCategory = async function(id) {
 await api(
  "/api/categories/update",
  form({
   id,
   name:$(`categoryName-${id}`).value,
   color:$(`categoryColor-${id}`).value
  })
 );

 await loadLibrary();
 renderCategoryManager();
};

window.deleteCategory = async function(id) {
 const category = categoryById(id);

 if (!category) {
  return;
 }

 if (!confirm(
  `Delete category "${category.name}"?\n\nPhotos will not be deleted.`
 )) {
  return;
 }

 await api(
  "/api/categories/delete",
  form({id})
 );

 await loadLibrary();
 await refreshStatus();

 renderCategoryManager();
};

function openCategoryAssignment(
 photoIds,
 batchMode
) {
 categoryAssignmentPhotoIds = photoIds;
 categoryAssignmentIsBatch = batchMode;

 $("categoryAssignTitle").textContent =
  batchMode
   ? `Assign Categories to ${photoIds.length} Photos`
   : "Photo Categories";

 $("categoryOperationField").classList.toggle(
  "hidden",
  !batchMode
 );

 if (!batchMode) {
  $("categoryOperation").value =
   "replace";
 }

 let checkedIds = [];

 if (
  !batchMode &&
  photoIds.length === 1
 ) {
  const photo = library.find(
   item => item.id === photoIds[0]
  );

  if (photo) {
   checkedIds =
    categoryIdsArray(photo.categoryIds);
  }
 }

 $("categoryAssignChoices").innerHTML =
  categories.length
   ? categories.map(category => `
      <label class="category-option">
       <input
        class="assign-category-check"
        type="checkbox"
        value="${category.id}"
        ${checkedIds.includes(category.id) ? "checked" : ""}
       >

       <span
        class="category-dot"
        style="background:${escapeHtml(category.color)}">
       </span>

       <span>${escapeHtml(category.name)}</span>
      </label>
     `).join("")
   : '<div class="muted">Create a category first.</div>';

 openOverlay("categoryAssignOverlay");
}


// ============================================================
// PHOTO LIBRARY
// ============================================================

const thumbnailObserver =
 new IntersectionObserver(
  entries => {
   entries.forEach(entry => {
    if (entry.isIntersecting) {
     thumbnailObserver.unobserve(
      entry.target
     );

     renderStoredCard(
      entry.target
     );
    }
   });
  },
  {rootMargin: "180px"}
 );

async function loadLibrary() {
 try {
  const data =
   await api(
    "/api/photos?_=" + Date.now()
   );

  library = data.photos;
  categories = data.categories || [];

  settings.mount = data.panelMount;

  libraryFilter =
   data.libraryFilter || "all";

  slideshowCategoryIds =
   data.slideshowCategoryIds || "";

  categoryMatchValue =
   data.categoryMatch === "all"
    ? "all"
    : "any";

  $("panelMount").value =
   settings.mount;

  const usedMB =
   (data.used / 1048576).toFixed(1);

  const freeMB =
   (data.free / 1048576).toFixed(1);

  const total =
   data.used + data.free;

  const percent =
   total
    ? data.used / total * 100
    : 0;

  $("storageText").textContent =
   `${data.count} photos · ${usedMB} MB used · ${freeMB} MB free`;

  $("storageDetails").textContent =
   `${data.count} stored photos. ${usedMB} MB used, ${freeMB} MB available.`;

  $("storageProgress").style.width =
   percent + "%";

  renderCategoryFilters();
  renderSlideshowCategoryChoices();
  renderLibrary();
 } catch (error) {
  console.error(error);
 }
}

function renderLibrary() {
 const visiblePhotos =
  library.filter(photo =>
   photoPassesLibraryFilter(photo)
  );

 $("library").classList.toggle(
  "hidden",
  libraryMode === "reorder"
 );

 $("reorderList").classList.toggle(
  "hidden",
  libraryMode !== "reorder"
 );

 $("selectionBar").classList.toggle(
  "visible",
  libraryMode === "select"
 );

 $("browseMode").className =
  libraryMode === "browse"
   ? "primary"
   : "secondary";

 $("selectMode").className =
  libraryMode === "select"
   ? "primary"
   : "secondary";

 $("reorderMode").className =
  libraryMode === "reorder"
   ? "primary"
   : "secondary";

 if (libraryMode === "reorder") {
  $("reorderList").innerHTML =
   library.map((photo, index) => `
    <div class="reorder-row">
     <span>
      ${index + 1}. ${escapeHtml(photo.name)}
     </span>

     <button
      class="secondary"
      onclick="movePhoto('${photo.id}',-1)"
      ${index === 0 ? "disabled" : ""}>
      ↑
     </button>

     <button
      class="secondary"
      onclick="movePhoto('${photo.id}',1)"
      ${index === library.length - 1 ? "disabled" : ""}>
      ↓
     </button>
    </div>
   `).join("") ||
   '<div class="muted">No photos stored.</div>';

  return;
 }

 $("library").innerHTML =
  visiblePhotos.map(photo => `
   <article
    class="photo ${photo.selected ? "selected-card" : ""}"
    data-id="${photo.id}"
   >
    ${
     libraryMode === "select"
      ? `<input
          class="select-box slide-check"
          type="checkbox"
          data-id="${photo.id}"
          ${photo.selected ? "checked" : ""}
         >`
      : ""
    }

    ${
     libraryMode === "browse"
      ? `<button
          class="menu-button"
          onclick="event.stopPropagation();openPhotoMenu('${photo.id}')">
          •••
         </button>`
      : ""
    }

    <canvas
     width="600"
     height="400"
     onclick="${
      libraryMode === "select"
       ? `toggleLibraryCheck('${photo.id}')`
       : `openPhotoDetail('${photo.id}')`
     }"
    ></canvas>

    <div class="photo-body">
     <div class="photo-name">
      ${escapeHtml(photo.name)}
     </div>

     ${renderCategoryChips(photo)}

     <div class="photo-meta">
      ${
       photo.selected
        ? "Included in slideshow"
        : "Not in slideshow"
      }
     </div>
    </div>
   </article>
  `).join("") ||
  '<div class="muted">No photos match this category.</div>';

 document
  .querySelectorAll(".photo")
  .forEach(card =>
   thumbnailObserver.observe(card)
  );

 document
  .querySelectorAll(".slide-check")
  .forEach(input =>
   input.onchange = updateSelectionCount
  );

 updateSelectionCount();
}

async function decodeStoredPhoto(
 photo,
 canvas
) {
 const response =
  await fetch(
   "/api/photo?id=" +
   photo.id
  );

 if (!response.ok) {
  return;
 }

 const bytes =
  new Uint8Array(
   await response.arrayBuffer()
  );

 if (bytes.length !== BYTES) {
  return;
 }

 const context =
  canvas.getContext("2d");

 const image =
  context.createImageData(W, H);

 let outputPixel = 0;

 for (
  let panelY = 0;
  panelY < PH;
  panelY++
 ) {
  for (
   let panelX = 0;
   panelX < PW;
   panelX++
  ) {
   const byte =
    bytes[outputPixel >> 1];

   const code =
    outputPixel & 1
     ? byte & 15
     : byte >> 4;

   outputPixel++;

   let sourceX;
   let sourceY;

   if (photo.mount === "right") {
    sourceX = panelY;
    sourceY = H - 1 - panelX;
   } else {
    sourceX = W - 1 - panelY;
    sourceY = panelX;
   }

   const rgb =
    colors[code] ||
    colors[1];

   const index =
    (
     sourceY * W +
     sourceX
    ) * 4;

   image.data[index] =
    rgb[0];

   image.data[index + 1] =
    rgb[1];

   image.data[index + 2] =
    rgb[2];

   image.data[index + 3] =
    255;
  }
 }

 context.putImageData(
  image,
  0,
  0
 );
}

async function renderStoredCard(card) {
 const photo =
  library.find(
   item =>
    item.id === card.dataset.id
  );

 if (photo) {
  await decodeStoredPhoto(
   photo,
   card.querySelector("canvas")
  );
 }
}


$("categoryFilter").onchange = async event => {
 libraryFilter = event.target.value;

 await api(
  "/api/library/filter",
  form({
   filter:libraryFilter
  })
 );

 renderLibrary();
};

$("manageCategories").onclick = () => {
 renderCategoryManager();
 openOverlay("categoryManagerOverlay");
};

$("createCategory").onclick = async () => {
 const name =
  $("newCategoryName").value.trim();

 if (!name) {
  alert("Enter a category name.");
  return;
 }

 await api(
  "/api/categories/create",
  form({
   name,
   color:$("newCategoryColor").value
  })
 );

 $("newCategoryName").value = "";

 await loadLibrary();
 renderCategoryManager();
};

$("assignSelectedCategories").onclick = () => {
 const ids = [
  ...document.querySelectorAll(
   ".slide-check:checked"
  )
 ].map(input => input.dataset.id);

 if (!ids.length) {
  alert("Check at least one photo.");
  return;
 }

 openCategoryAssignment(ids, true);
};

$("menuCategories").onclick = () => {
 closeOverlays();

 openCategoryAssignment(
  [activePhotoId],
  false
 );
};

$("saveCategoryAssignment").onclick = async () => {
 const categoryIds = [
  ...document.querySelectorAll(
   ".assign-category-check:checked"
  )
 ].map(input => input.value).join(",");

 const operation =
  categoryAssignmentIsBatch
   ? $("categoryOperation").value
   : "replace";

 await api(
  "/api/photos/categories",
  form({
   ids:categoryAssignmentPhotoIds.join(","),
   categoryIds,
   operation
  })
 );

 closeOverlays();

 await loadLibrary();
 await refreshStatus();
};

$("categoryMatch").onchange = () => {
 categoryMatchValue =
  $("categoryMatch").value === "all"
   ? "all"
   : "any";

 slideshowDirty = true;
 updateSlideshowPoolSummary();
};

$("browseMode").onclick = () => {
 libraryMode = "browse";
 renderLibrary();
};

$("selectMode").onclick = () => {
 libraryMode = "select";
 renderLibrary();
};

$("reorderMode").onclick = () => {
 libraryMode = "reorder";
 renderLibrary();
};

window.toggleLibraryCheck = function(id) {
 const checkbox =
  document.querySelector(
   `.slide-check[data-id="${id}"]`
  );

 if (checkbox) {
  checkbox.checked =
   !checkbox.checked;

  updateSelectionCount();
 }
};

function updateSelectionCount() {
 const count =
  document.querySelectorAll(
   ".slide-check:checked"
  ).length;

 $("selectionCount").textContent =
  `${count} checked`;
}

$("selectAll").onclick = () => {
 document
  .querySelectorAll(".slide-check")
  .forEach(input =>
   input.checked = true
  );

 updateSelectionCount();
};

$("selectNone").onclick = () => {
 document
  .querySelectorAll(".slide-check")
  .forEach(input =>
   input.checked = false
  );

 updateSelectionCount();
};

$("invertSelection").onclick = () => {
 document
  .querySelectorAll(".slide-check")
  .forEach(input =>
   input.checked =
    !input.checked
  );

 updateSelectionCount();
};

$("applySelection").onclick = async () => {
 const ids = [
  ...document.querySelectorAll(
   ".slide-check:checked"
  )
 ].map(input =>
  input.dataset.id
 ).join(",");

 await api(
  "/api/photos/select",
  form({ids})
 );

 await loadLibrary();
 await refreshStatus();
};

$("deleteSelected").onclick = async () => {
 const checkedInputs = [
  ...document.querySelectorAll(
   ".slide-check:checked"
  )
 ];

 if (!checkedInputs.length) {
  alert(
   "Select at least one photo to delete."
  );

  return;
 }

 const ids =
  checkedInputs.map(
   input => input.dataset.id
  );

 const selectedPhotos =
  ids
   .map(id =>
    library.find(
     photo => photo.id === id
    )
   )
   .filter(Boolean);

 const previewNames =
  selectedPhotos
   .slice(0, 5)
   .map(photo =>
    "• " + photo.name
   )
   .join("\n");

 let message =
  `Permanently delete ${ids.length} photo`;

 if (ids.length !== 1) {
  message += "s";
 }

 message += "?\n\n" + previewNames;

 if (selectedPhotos.length > 5) {
  message +=
   `\n• and ${selectedPhotos.length - 5} more`;
 }

 message +=
  "\n\nThis cannot be undone.";

 if (!confirm(message)) {
  return;
 }

 if (
  ids.length >= 10 &&
  !confirm(
   `Confirm deletion of all ${ids.length} checked photos.`
  )
 ) {
  return;
 }

 const button = $("deleteSelected");
 const originalText = button.textContent;

 button.disabled = true;
 button.textContent = "Deleting…";

 try {
  const response = await api(
   "/api/photos/delete-batch",
   form({
    ids: ids.join(",")
   })
  );

  await loadLibrary();
  await refreshStatus();

  alert(
   `${response.deleted} photo` +
   (response.deleted === 1 ? "" : "s") +
   " deleted."
  );
 } catch (error) {
  alert(
   "Could not delete photos:\n\n" +
   error.message
  );
 } finally {
  button.disabled = false;
  button.textContent = originalText;
 }
};

window.openPhotoMenu = function(id) {
 const photo =
  library.find(
   item => item.id === id
  );

 if (!photo) {
  return;
 }

 activePhotoId = id;

 $("menuPhotoName").textContent =
  photo.name;

 $("menuToggleSelection").textContent =
  photo.selected
   ? "Exclude from Slideshow"
   : "Include in Slideshow";

 openOverlay("menuOverlay");
};

window.openPhotoDetail = async function(id) {
 const photo =
  library.find(
   item => item.id === id
  );

 if (!photo) {
  return;
 }

 activePhotoId = id;

 $("detailName").textContent =
  photo.name;

 $("detailMeta").textContent =
  `${
   photo.selected
    ? "Included"
    : "Not included"
  } in slideshow · 120,000 bytes`;

 const canvas =
  $("detailCanvas");

 canvas
  .getContext("2d")
  .clearRect(0, 0, W, H);

 openOverlay("detailOverlay");

 await decodeStoredPhoto(
  photo,
  canvas
 );
};

async function displayNow(id) {
 if (displayBusy) {
  alert(
   "The display is currently busy."
  );

  return;
 }

 await api(
  "/api/slideshow/action",
  form({
   action: "display",
   id
  })
 );

 closeOverlays();
 await refreshStatus();
}

$("menuDisplay").onclick =
 () => displayNow(activePhotoId);

$("detailDisplay").onclick =
 () => displayNow(activePhotoId);

$("menuToggleSelection").onclick =
 async () => {
  const photo =
   library.find(
    item =>
     item.id === activePhotoId
   );

  if (!photo) {
   return;
  }

  const ids =
   library
    .filter(item =>
     item.id === photo.id
      ? !item.selected
      : item.selected
    )
    .map(item => item.id)
    .join(",");

  await api(
   "/api/photos/select",
   form({ids})
  );

  closeOverlays();

  await loadLibrary();
  await refreshStatus();
 };

$("menuRename").onclick =
 async () => {
  const photo =
   library.find(
    item =>
     item.id === activePhotoId
   );

  if (!photo) {
   return;
  }

  const name =
   prompt(
    "Photo name",
    photo.name
   );

  if (name !== null) {
   await api(
    "/api/photos/rename",
    form({
     id: photo.id,
     name
    })
   );

   closeOverlays();
   await loadLibrary();
  }
 };

$("menuMoveUp").onclick = () => {
 movePhoto(activePhotoId, -1);
 closeOverlays();
};

$("menuMoveDown").onclick = () => {
 movePhoto(activePhotoId, 1);
 closeOverlays();
};

$("menuDelete").onclick =
 async () => {
  const photo =
   library.find(
    item =>
     item.id === activePhotoId
   );

  if (!photo) {
   return;
  }

  if (confirm(
   `Delete "${photo.name}"?`
  )) {
   await api(
    "/api/photos/delete",
    form({
     id: photo.id
    })
   );

   closeOverlays();

   await loadLibrary();
   await refreshStatus();
  }
 };

window.movePhoto =
 async function(id, direction) {
  try {
   await api(
    "/api/photos/move",
    form({
     id,
     direction
    })
   );

   await loadLibrary();
  } catch (error) {
   console.log(error.message);
  }
 };

// ============================================================
// SLIDESHOW UI
// ============================================================

$("slideInterval").oninput = () => {
 slideshowDirty = true;
};

$("slideMode").onchange = () => {
 slideshowDirty = true;
};

document
 .querySelectorAll(".intervalPreset")
 .forEach(button => {
  button.onclick = () => {
   $("slideInterval").value =
    button.dataset.seconds;

   slideshowDirty = true;
  };
 });

$("saveSlideshow").onclick =
 async () => {
  if (slideshowSaving) {
   return;
  }

  let interval =
   parseInt(
    $("slideInterval").value,
    10
   );

  if (!Number.isFinite(interval)) {
   interval = 300;
  }

  interval =
   Math.max(30, interval);

  const mode =
   $("slideMode").value === "shuffle"
    ? "shuffle"
    : "sequential";

  $("slideInterval").value =
   interval;

  slideshowSaving = true;
  $("saveSlideshow").disabled = true;

  try {
   const response =
    await api(
     "/api/slideshow/settings",
     form({
      interval: String(interval),
      mode,
      categoryIds:slideshowCategoryIds,
      categoryMatch:categoryMatchValue
     })
    );

   $("slideInterval").value =
    response.interval;

   $("slideMode").value =
    response.mode;

   slideshowDirty = false;

   await refreshStatus(true);
  } catch (error) {
   alert(error.message);
  } finally {
   slideshowSaving = false;
   $("saveSlideshow").disabled = false;
  }
 };

$("toggleSlide").onclick =
 async () => {
  if (displayBusy) {
   return;
  }

  const action =
   $("toggleSlide").dataset.running === "1"
    ? "pause"
    : "start";

  try {
   await api(
    "/api/slideshow/action",
    form({action})
   );

   await refreshStatus();
  } catch (error) {
   alert(error.message);
  }
 };

$("nextSlide").onclick =
 async () => {
  if (displayBusy) {
   return;
  }

  try {
   await api(
    "/api/slideshow/action",
    form({action: "next"})
   );

   await refreshStatus();
  } catch (error) {
   alert(error.message);
  }
 };

$("previousSlide").onclick =
 async () => {
  if (displayBusy) {
   return;
  }

  try {
   await api(
    "/api/slideshow/action",
    form({action: "previous"})
   );

   await refreshStatus();
  } catch (error) {
   alert(error.message);
  }
 };


// ============================================================
// MUTUALLY EXCLUSIVE EXTENSION DASHBOARDS
// ============================================================

function hideAllExtensionDashboards() {
 const dashboardIds = [
  "clockWeatherDashboard",
  "calendarDashboard",
  "informationBoardDashboard"
 ];

 dashboardIds.forEach(
  dashboardId => {
   const dashboard =
    document.getElementById(dashboardId);

   if (dashboard) {
    dashboard.classList.add("hidden");
   }
  }
 );
}

/*
 Run before the existing dashboard button handlers.

 The existing handler remains responsible for loading and
 displaying the selected dashboard. This listener makes sure
 settings from another extension cannot remain visible.
*/
document.addEventListener(
 "click",
 event => {
  const openButton = event.target.closest(
   "#openClockWeather, " +
   "#openCalendar, " +
   "#openInformationBoard"
  );

  if (!openButton) {
   return;
  }

  hideAllExtensionDashboards();
 },
 true
);


// ============================================================
// CLOCK & WEATHER EXTENSION UI
// ============================================================

let clockWeatherLoaded = false;
let clockWeatherStatus = null;

async function decodeExtensionPreview() {
 const response = await fetch(
  "/api/extensions/clock-weather/preview?_=" +
  Date.now(),
  {cache:"no-store"}
 );

 if (!response.ok) {
  throw Error(
   await response.text()
  );
 }

 const bytes =
  new Uint8Array(
   await response.arrayBuffer()
  );

 if (bytes.length !== BYTES) {
  throw Error(
   "Dashboard preview has an invalid size."
  );
 }

 const canvas =
  $("cwPreviewCanvas");

 const context =
  canvas.getContext("2d");

 const image =
  context.createImageData(W,H);

 let outputPixel = 0;

 for (let panelY = 0;
      panelY < PH;
      panelY++) {
  for (let panelX = 0;
       panelX < PW;
       panelX++) {
   const byte =
    bytes[outputPixel >> 1];

   const code =
    outputPixel & 1
     ? byte & 15
     : byte >> 4;

   outputPixel++;

   let sourceX;
   let sourceY;

   if (settings.mount === "right") {
    sourceX = panelY;
    sourceY = H - 1 - panelX;
   } else {
    sourceX = W - 1 - panelY;
    sourceY = panelX;
   }

   const rgb =
    colors[code] ||
    colors[1];

   const index =
    (
     sourceY * W +
     sourceX
    ) * 4;

   image.data[index] = rgb[0];
   image.data[index + 1] = rgb[1];
   image.data[index + 2] = rgb[2];
   image.data[index + 3] = 255;
  }
 }

 context.putImageData(
  image,
  0,
  0
 );
}

async function loadClockWeather() {
 const status = await api(
  "/api/extensions/clock-weather/status?_=" +
  Date.now()
 );

 clockWeatherStatus = status;
 clockWeatherLoaded = true;

 $("cwLayout").value =
  status.layout;

 $("cwLocation").value =
  status.location;

 $("cwLatitude").value =
  status.latitude;

 $("cwLongitude").value =
  status.longitude;

 $("cwTimezone").value =
  status.timezone;

 $("cwTemperatureUnit").value =
  status.temperatureUnit;

 $("cwWindUnit").value =
  status.windUnit;

 $("cwInterval").value =
  String(status.interval);

 $("cwWeatherInterval").value =
  String(status.weatherInterval);

 $("cwTwelveHour").checked =
  status.twelveHour !== false;

 $("cwActiveStatus").textContent =
  status.active
   ? "Active"
   : "Inactive";

 $("cwCurrentTime").textContent =
  status.currentTime;

 $("cwWeatherStatus").textContent =
  status.weatherValid
   ? `${Math.round(status.temperature)}° · ${status.weatherDescription}`
   : (
      status.weatherError
       ? `Unavailable · ${status.weatherError}`
       : "Unavailable"
     );

 $("cwActivate").disabled =
  status.active;

 $("cwDeactivate").disabled =
  !status.active;

 await decodeExtensionPreview();
}

async function clockWeatherAction(action) {
 await api(
  "/api/extensions/clock-weather/action",
  form({action})
 );

 await loadClockWeather();
 await refreshStatus();
}

$("openClockWeather").onclick = async () => {
 $("clockWeatherDashboard")
  .classList.remove("hidden");

 await loadClockWeather();
};

$("closeClockWeather").onclick = () => {
 $("clockWeatherDashboard")
  .classList.add("hidden");
};

$("cwRefreshPreview").onclick = async () => {
 await decodeExtensionPreview();
};


async function searchClockWeatherLocation() {
 const query =
  $("cwLocationSearch").value.trim();

 if (query.length < 2) {
  alert(
   "Enter at least two characters."
  );

  return;
 }

 $("cwSearchLocation").disabled = true;
 $("cwSearchLocation").textContent =
  "Searching…";

 $("cwSearchStatus").textContent =
  "Searching worldwide locations…";

 try {
  const data = await api(
   "/api/extensions/clock-weather/search" +
   "?query=" +
   encodeURIComponent(query) +
   "&_=" +
   Date.now()
  );

  const results =
   data.results || [];

  if (!results.length) {
   $("cwSearchResults").innerHTML =
    '<option value="">No matching locations</option>';

   $("cwSearchStatus").textContent =
    "No matching locations were found.";

   return;
  }

  $("cwSearchResults").innerHTML =
   '<option value="">Choose a location</option>' +
   results.map((result,index) => {
    const label = [
     result.name,
     result.admin1,
     result.country
    ].filter(Boolean).join(", ");

    return `
     <option
      value="${index}"
      data-name="${escapeHtml(result.name || "")}"
      data-latitude="${result.latitude}"
      data-longitude="${result.longitude}"
      data-timezone="${escapeHtml(result.timezone || "auto")}">
      ${escapeHtml(label)}
     </option>
    `;
   }).join("");

  $("cwSearchStatus").textContent =
   `${results.length} global locations found.`;

 } catch (error) {
  $("cwSearchStatus").textContent =
   "Search failed: " + error.message;

  alert(
   "Location search failed:\n\n" +
   error.message
  );
 } finally {
  $("cwSearchLocation").disabled = false;
  $("cwSearchLocation").textContent =
   "Search";
 }
}

$("cwSearchLocation").onclick =
 searchClockWeatherLocation;

$("cwLocationSearch").onkeydown =
 event => {
  if (event.key === "Enter") {
   event.preventDefault();
   searchClockWeatherLocation();
  }
 };

$("cwSearchResults").onchange =
 event => {
  const option =
   event.target.selectedOptions[0];

  if (
   !option ||
   !option.value
  ) {
   return;
  }

  $("cwLocation").value =
   option.dataset.name ||
   option.textContent.trim();

  $("cwLatitude").value =
   Number(
    option.dataset.latitude
   ).toFixed(6);

  $("cwLongitude").value =
   Number(
    option.dataset.longitude
   ).toFixed(6);

  $("cwTimezone").value =
   option.dataset.timezone ||
   "auto";

  $("cwSearchStatus").textContent =
   "Location selected. Press Save Dashboard Settings.";
};

$("cwUseLocation").onclick = () => {
 if (!window.isSecureContext) {
  alert(
   "Browser geolocation is blocked on this HTTP page.\n\n" +
   "Use the global city search above instead. Search for any " +
   "city, district or postal code worldwide."
  );

  return;
 }

 if (!navigator.geolocation) {
  alert(
   "Browser location is not supported. Use global city search."
  );

  return;
 }

 navigator.geolocation.getCurrentPosition(
  position => {
   $("cwLatitude").value =
    position.coords.latitude.toFixed(6);

   $("cwLongitude").value =
    position.coords.longitude.toFixed(6);

   $("cwLocation").value =
    "Current Location";

   $("cwTimezone").value =
    "auto";

   $("cwSearchStatus").textContent =
    "Browser location selected. Press Save Dashboard Settings.";
  },
  error => {
   alert(
    "Location permission was not granted.\n\n" +
    "Use the global city search instead.\n\n" +
    error.message
   );
  },
  {
   enableHighAccuracy:false,
   timeout:10000,
   maximumAge:300000
  }
 );
};

$("cwSave").onclick = async () => {
 await api(
  "/api/extensions/clock-weather/settings",
  form({
   layout:$("cwLayout").value,
   location:$("cwLocation").value,
   latitude:$("cwLatitude").value,
   longitude:$("cwLongitude").value,
   timezone:$("cwTimezone").value,
   temperatureUnit:
    $("cwTemperatureUnit").value,
   windUnit:$("cwWindUnit").value,
   interval:$("cwInterval").value,
   weatherInterval:
    $("cwWeatherInterval").value,
   twelveHour:
    $("cwTwelveHour").checked
     ? "1"
     : "0"
  })
 );

 await loadClockWeather();

 alert(
  "Clock & Weather settings saved."
 );
};

$("cwDisplayNow").onclick = () =>
 clockWeatherAction("display-now");

$("cwActivate").onclick = () =>
 clockWeatherAction("activate");

$("cwDeactivate").onclick = () =>
 clockWeatherAction("deactivate");

$("cwSyncTime").onclick = () =>
 clockWeatherAction("sync-time");

$("cwRefreshWeather").onclick = () =>
 clockWeatherAction("refresh-weather");




// ============================================================
// CALENDAR EXTENSION UI
// ============================================================

let calendarStatus = null;

async function decodeCalendarPreview() {
 const response = await fetch(
  "/api/extensions/calendar/preview?_=" +
  Date.now(),
  {cache:"no-store"}
 );

 if (!response.ok) {
  throw Error(
   await response.text()
  );
 }

 const bytes =
  new Uint8Array(
   await response.arrayBuffer()
  );

 if (bytes.length !== BYTES) {
  throw Error(
   "Invalid calendar preview."
  );
 }

 const canvas =
  $("calPreviewCanvas");

 const context =
  canvas.getContext("2d");

 const image =
  context.createImageData(W,H);

 let outputPixel = 0;

 for (
  let panelY = 0;
  panelY < PH;
  panelY++
 ) {
  for (
   let panelX = 0;
   panelX < PW;
   panelX++
  ) {
   const byte =
    bytes[outputPixel >> 1];

   const code =
    outputPixel & 1
     ? byte & 15
     : byte >> 4;

   outputPixel++;

   let sourceX;
   let sourceY;

   if (settings.mount === "right") {
    sourceX = panelY;
    sourceY = H - 1 - panelX;
   } else {
    sourceX = W - 1 - panelY;
    sourceY = panelX;
   }

   const rgb =
    colors[code] ||
    colors[1];

   const index =
    (
     sourceY * W +
     sourceX
    ) * 4;

   image.data[index] = rgb[0];
   image.data[index + 1] = rgb[1];
   image.data[index + 2] = rgb[2];
   image.data[index + 3] = 255;
  }
 }

 context.putImageData(
  image,
  0,
  0
 );
}

function renderCalendarFeeds() {
 $("calendarFeeds").innerHTML =
  calendarStatus.feeds.map(feed => `
   <div class="calendar-feed">

    <div class="calendar-feed-grid">
     <label class="checkline">
      <input
       id="calFeedEnabled-${feed.index}"
       type="checkbox"
       ${feed.enabled ? "checked" : ""}>

      <span>Enabled</span>
     </label>

     <div>
      <label>Name</label>

      <input
       id="calFeedName-${feed.index}"
       type="text"
       value="${escapeHtml(feed.name)}">
     </div>

     <div>
      <label>Color</label>

      <input
       id="calFeedColor-${feed.index}"
       type="color"
       value="${escapeHtml(feed.color)}">
     </div>
    </div>

    <div class="field">
     <label>
      ICS URL ${feed.configured ? "· Saved" : ""}
     </label>

     <input
      id="calFeedUrl-${feed.index}"
      type="password"
      placeholder="${
       feed.configured
        ? "Blank keeps existing URL"
        : "https://...calendar.ics"
      }">
    </div>

    <div class="row" style="margin-top:9px">
     <button
      class="secondary"
      onclick="saveCalendarFeed(${feed.index})">
      Save Feed
     </button>

     <button
      class="danger"
      onclick="deleteCalendarFeed(${feed.index})">
      Remove Feed
     </button>
    </div>
   </div>
  `).join("");
}

async function loadCalendar() {
 const status = await api(
  "/api/extensions/calendar/status?_=" +
  Date.now()
 );

 calendarStatus = status;

 $("calLayout").value =
  status.layout;

 $("calRange").value =
  String(status.rangeDays);

 $("calMaximumEvents").value =
  status.maximumEvents;

 $("calPrivacy").value =
  status.privacy;

 $("calOffset").value =
  status.utcOffsetMinutes;

 $("calSyncInterval").value =
  String(status.syncInterval);

 $("calActiveStatus").textContent =
  status.active
   ? "Active"
   : "Inactive";

 $("calEventCount").textContent =
  status.eventCount;

 $("calSyncStatus").textContent =
  status.lastError
   ? status.lastError
   : "Ready";

 $("calActivate").disabled =
  status.active;

 $("calDeactivate").disabled =
  !status.active;

 renderCalendarFeeds();

 await decodeCalendarPreview();
}

window.saveCalendarFeed =
 async function(index) {
  await api(
   "/api/extensions/calendar/feed",
   form({
    index:String(index),

    enabled:
     $(`calFeedEnabled-${index}`).checked
      ? "1"
      : "0",

    name:
     $(`calFeedName-${index}`).value,

    url:
     $(`calFeedUrl-${index}`).value,

    color:
     $(`calFeedColor-${index}`).value
   })
  );

  await loadCalendar();

  alert(
   "Calendar feed saved."
  );
};

window.deleteCalendarFeed =
 async function(index) {
  if (!confirm(
   "Remove this calendar feed?"
  )) {
   return;
  }

  await api(
   "/api/extensions/calendar/feed/delete",
   form({
    index:String(index)
   })
  );

  await loadCalendar();
};

async function calendarAction(action) {
 await api(
  "/api/extensions/calendar/action",
  form({action})
 );

 await loadCalendar();
 await refreshStatus();
}

$("openCalendar").onclick =
 async () => {
  $("calendarDashboard")
   .classList
   .remove("hidden");

  await loadCalendar();
};

$("closeCalendar").onclick =
 () => {
  $("calendarDashboard")
   .classList
   .add("hidden");
};

$("calRefreshPreview").onclick =
 decodeCalendarPreview;

$("calSaveSettings").onclick =
 async () => {
  await api(
   "/api/extensions/calendar/settings",
   form({
    layout:
     $("calLayout").value,

    rangeDays:
     $("calRange").value,

    maximumEvents:
     $("calMaximumEvents").value,

    privacy:
     $("calPrivacy").value,

    utcOffsetMinutes:
     $("calOffset").value,

    syncInterval:
     $("calSyncInterval").value
   })
  );

  await loadCalendar();

  alert(
   "Calendar settings saved."
  );
};

$("calDisplayNow").onclick =
 () => calendarAction("display-now");

$("calActivate").onclick =
 () => calendarAction("activate");

$("calDeactivate").onclick =
 () => calendarAction("deactivate");

$("calSyncNow").onclick =
 () => calendarAction("sync");



// ============================================================
// SETTINGS UI
// ============================================================

$("hostname").oninput = () => {
 $("hostnamePreview").textContent =
  $("hostname").value ||
  "dynapix";
};

$("saveDevice").onclick =
 async () => {
  const oldMount =
   settings.mount;

  const newMount =
   $("panelMount").value;

  await api(
   "/api/settings/device",
   form({
    name:
     $("deviceName").value,

    hostname:
     $("hostname").value,

    mount:
     newMount
   })
  );

  settings.mount = newMount;

  let message =
   "Device settings saved. Hostname changes take effect after restart.";

  if (
   oldMount !== newMount &&
   library.length
  ) {
   message +=
    "\n\nExisting photos keep their previous orientation.";
  }

  alert(message);
 };

$("scanNetworks").onclick =
 async () => {
  $("scanNetworks").disabled =
   true;

  $("scanNetworks").textContent =
   "Scanning…";

  try {
   const data =
    await api("/api/wifi/scan");

   $("networkList").innerHTML =
    '<option value="">Choose a network</option>' +
    data.networks.map(network => `
     <option value="${escapeHtml(network.ssid)}">
      ${escapeHtml(network.ssid)} (${network.rssi} dBm)
     </option>
    `).join("");
  } catch (error) {
   alert(error.message);
  } finally {
   $("scanNetworks").disabled =
    false;

   $("scanNetworks").textContent =
    "Scan Wi-Fi";
  }
 };

$("networkList").onchange =
 event => {
  if (event.target.value) {
   $("wifiSSID").value =
    event.target.value;
  }
 };

$("openNetwork").onchange = () => {
 $("wifiPassword").disabled =
  $("openNetwork").checked;
};

$("saveWiFi").onclick =
 async () => {
  const ssid =
   $("wifiSSID").value.trim();

  if (!ssid) {
   alert(
    "Enter a Wi-Fi name."
   );

   return;
  }

  if (!confirm(
   "Save Wi-Fi settings and restart the ESP32?"
  )) {
   return;
  }

  await api(
   "/api/settings/wifi",
   form({
    ssid,

    password:
     $("wifiPassword").value,

    open:
     $("openNetwork").checked
      ? "1"
      : "0"
   })
  );

  alert(
   "ESP32 is restarting. Reconnect using dynapix.local or DynaPix-EPaper."
  );
 };

$("cleanStorage").onclick =
 async () => {
  await api(
   "/api/maintenance/cleanup",
   form({})
  );

  await loadLibrary();

  alert(
   "Storage cleanup completed."
  );
 };

$("restartDevice").onclick =
 async () => {
  if (!confirm(
   "Restart the ESP32?"
  )) {
   return;
  }

  await api(
   "/api/maintenance/restart",
   form({})
  );
 };

$("resetSlideshow").onclick =
 async () => {
  if (!confirm(
   "Reset slideshow to 300 seconds and Sequential?"
  )) {
   return;
  }

  await api(
   "/api/maintenance/reset-slideshow",
   form({})
  );

  slideshowDirty = false;

  await refreshStatus(true);
 };

$("factoryReset").onclick =
 async () => {
  if (!confirm(
   "FACTORY RESET?\n\nThis deletes all photos and settings."
  )) {
   return;
  }

  if (!confirm(
   "Are you absolutely sure?"
  )) {
   return;
  }

  await api(
   "/api/maintenance/factory-reset",
   form({})
  );
 };

// ============================================================
// STATUS UI
// ============================================================

function applyBusyState(busy) {
 displayBusy = busy;

 $("previousSlide").disabled = busy;
 $("nextSlide").disabled = busy;
 $("toggleSlide").disabled = busy;
 $("menuDisplay").disabled = busy;
 $("detailDisplay").disabled = busy;

 $("busyBadge").textContent =
  busy
   ? "Display refreshing…"
   : "";
}

async function refreshStatus(
 forceFields = false
) {
 try {
  const status =
   await api(
    "/api/status?_=" +
    Date.now()
   );

  $("connection").textContent =
   status.stationConnected
    ? `Connected · ${status.ip}`
    : `Fallback AP · ${status.apIP}`;

  $("deviceTitle").textContent =
   "DynaPix";

  $("slideState").textContent =
   status.displayBusy
    ? "Refreshing"
    : status.slideshowRunning
     ? "Running"
     : "Paused";

  $("currentPhoto").textContent =
   status.currentName ||
   "None";

  $("countdown").textContent =
   status.slideshowRunning
    ? status.nextSeconds + " sec"
    : "--";

  $("selectedSummary").textContent =
   `${status.eligibleCount} active · ${status.selectedCount} enabled · ${status.photoCount} total`;

  if (!slideshowDirty) {
   slideshowCategoryIds =
    status.slideshowCategoryIds || "";

   categoryMatchValue =
    status.categoryMatch === "all"
     ? "all"
     : "any";

   renderSlideshowCategoryChoices();
  }

  $("toggleSlide").textContent =
   status.slideshowRunning
    ? "Pause"
    : "Start";

  $("toggleSlide").dataset.running =
   status.slideshowRunning
    ? "1"
    : "0";

  if (
   forceFields ||
   !slideshowDirty
  ) {
   $("slideInterval").value =
    Math.max(
     30,
     Number(status.interval) ||
     300
    );

   $("slideMode").value =
    status.mode === "shuffle"
     ? "shuffle"
     : "sequential";
  }

  $("deviceName").value =
   status.deviceName;

  $("hostname").value =
   status.hostname;

  $("hostnamePreview").textContent =
   status.hostname;

  $("wifiSSID").value =
   status.ssid ||
   "";

  $("wifiStatus").textContent =
   status.stationConnected
    ? `Connected to ${status.ssid} · ${status.ip} · ${status.rssi} dBm`
    : `Router unavailable. Fallback access point: ${status.apIP}`;

  applyBusyState(
   status.displayBusy
  );
 } catch (error) {
  $("connection").textContent =
   "ESP32 busy or unavailable";
 }
}

applyQualityPreset("detail");
loadLibrary();
refreshStatus();

setInterval(() => {
 refreshStatus();
}, 3000);
</script>
</body>
</html>
)DPIXUI26";
