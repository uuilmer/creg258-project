/**
 * entry.js
 *
 * This is the first file loaded. It sets up the Renderer,
 * Scene and Camera. It also starts the render loop and
 * handles window resizes.
 *
 */

import * as THREE from "three";
import SeedScene from "./objects/Scene.js";

const scene = new THREE.Scene();
const camera = new THREE.PerspectiveCamera();
const renderer = new THREE.WebGLRenderer({ antialias: true });
const seedScene = new SeedScene();
var tmpPos = { x: 0, y: 0, z: 0 };
var tmpQuar = { real: 0, i: 0, j: 0, k: 0 };

var quaternion = new THREE.Quaternion();

// scene
scene.add(seedScene);

// camera
camera.position.set(0, 0, 10);
camera.lookAt(new THREE.Vector3(0, 0, 0));

// renderer
renderer.setPixelRatio(window.devicePixelRatio);
renderer.setClearColor(0x7ec0ee, 1);

const updateUSBQuaterion = () => {
  var usbQuarterion = {};

  usbQuarterion.real = tmpQuar.real;
  usbQuarterion.i = tmpQuar.i;
  usbQuarterion.j = tmpQuar.j;
  usbQuarterion.k = tmpQuar.k;
  // TODO: Retrieve new Quarterion via USB

  quaternion.set(
    usbQuarterion.real,
    usbQuarterion.i,
    usbQuarterion.j,
    usbQuarterion.k
  );
  //quaternion.setFromAxisAngle(new THREE.Vector3(1, 0, 0), Math.PI / 2);

  quaternion.normalize();

  // Apply orientation to seedScene
  seedScene.quaternion.copy(quaternion);
};

const updateUSBPosition = () => {
  var usbPosition = {};

  usbPosition.x = tmpPos.x;
  usbPosition.y = tmpPos.y;
  usbPosition.z = tmpPos.z;
  // TODO: Retrieve new position via USB

  seedScene.position.set(usbPosition.x, usbPosition.y, usbPosition.z);
};

// render loop
const onAnimationFrameHandler = (timeStamp) => {
  updateUSBQuaterion();
  updateUSBPosition();

  renderer.render(scene, camera);
  seedScene.update && seedScene.update(timeStamp);
  window.requestAnimationFrame(onAnimationFrameHandler);
};
window.requestAnimationFrame(onAnimationFrameHandler);

// resize
const windowResizeHanlder = () => {
  const { innerHeight, innerWidth } = window;
  renderer.setSize(innerWidth, innerHeight);
  camera.aspect = innerWidth / innerHeight;
  camera.updateProjectionMatrix();
};
windowResizeHanlder();
window.addEventListener("resize", windowResizeHanlder);

// START TRASH

var btn = document.createElement("button");
btn.innerHTML = "x+";
btn.onclick = function () {
  tmpPos.x++;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "x-";
btn.onclick = function () {
  tmpPos.x--;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "y+";
btn.onclick = function () {
  tmpPos.y++;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "y-";
btn.onclick = function () {
  tmpPos.y--;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "z+";
btn.onclick = function () {
  tmpPos.z++;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "z-";
btn.onclick = function () {
  tmpPos.z--;
};
document.body.appendChild(btn);

var btn = document.createElement("button");
btn.innerHTML = "real+";
btn.onclick = function () {
  tmpQuar.real++;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "real-";
btn.onclick = function () {
  tmpQuar.real--;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "i+";
btn.onclick = function () {
  tmpQuar.i++;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "i-";
btn.onclick = function () {
  tmpQuar.i--;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "j+";
btn.onclick = function () {
  tmpQuar.j++;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "j-";
btn.onclick = function () {
  tmpQuar.j--;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "k+";
btn.onclick = function () {
  tmpQuar.k++;
};
document.body.appendChild(btn);

btn = document.createElement("button");
btn.innerHTML = "k-";
btn.onclick = function () {
  tmpQuar.k--;
};
document.body.appendChild(btn);

// dom
document.body.style.margin = 0;
document.body.appendChild(renderer.domElement);
