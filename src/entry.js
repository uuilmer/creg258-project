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

var quaternion = new THREE.Quaternion();

var interfaceNumber;
var endpointOut;
var endpointIn;

var runningInputVariable = "";
var newUSBGyroscope = [];
var lastUpdate;

var stall = false;

var button = document.createElement("button");
button.innerHTML = "click me";
button.onclick = function () {
  navigator.usb.requestDevice({ filters: [] }).then(function (device) {
    let readLoop = () => {
      device.transferIn(endpointIn, 32).then(
        (result) => {
          var rawInput = new TextDecoder().decode(result.data);
          for (var c of rawInput) {
            if (c == ",") {
              newUSBGyroscope.push(parseFloat(runningInputVariable));
              updateUSBQuaterion();
              runningInputVariable = "";
              newUSBGyroscope = [];
            } else if (c == " ") {
              if (runningInputVariable == "") {
                continue;
              }
              newUSBGyroscope.push(parseFloat(runningInputVariable));
              runningInputVariable = "";
            } else {
              runningInputVariable = runningInputVariable.concat(c);
            }
          }
          readLoop();
        },
        (error) => {
          console.log(error);
        }
      );
    };

    device
      .open()
      .then(() => {
        if (device.configuration === null) {
          return device.selectConfiguration(1);
        }
      })
      .then(() => {
        var interfaces = device.configuration.interfaces;
        var element = interfaces[2];
        element.alternates.forEach((elementalt) => {
          if (elementalt.interfaceClass == 0xff) {
            interfaceNumber = element.interfaceNumber;
            elementalt.endpoints.forEach((elementendpoint) => {
              if (elementendpoint.direction == "out") {
                endpointOut = elementendpoint.endpointNumber;
              }
              if (elementendpoint.direction == "in") {
                endpointIn = elementendpoint.endpointNumber;
              }
            });
          }
        });
      })
      .then(() => device.claimInterface(interfaceNumber))
      .then(() => device.selectAlternateInterface(interfaceNumber, 0))
      .then(() =>
        device.controlTransferOut({
          requestType: "class",
          recipient: "interface",
          request: 0x22,
          value: 0x01,
          index: interfaceNumber,
        })
      )
      .then(() => {
        readLoop();
      });

    var button2 = document.createElement("button");
    button2.innerHTML = "read";
    button2.onclick = function () {
      if (device.opened) {
        var enc = new TextEncoder(); // always utf-8
        var promise = device.transferOut(
          endpointOut,
          enc.encode("Test message")
        );
        promise.then(function (res) {
          console.log("lmao sent");
          console.log(res);
        });
      }
      return false;
    };
    document.body.appendChild(button2);
  });
  return false;
};

// where do we want to have the button to appear?
// you can append it to another element just by doing something like
// document.getElementById('foobutton').appendChild(button);
document.body.appendChild(button);

// scene
scene.add(seedScene);

// camera
camera.position.set(-5, 0, 0);
camera.lookAt(new THREE.Vector3(0, 0, 0));

// renderer
renderer.setPixelRatio(window.devicePixelRatio);
renderer.setClearColor(0x7ec0ee, 1);

const updateUSBQuaterion = () => {
  if (lastUpdate) {
    var dt = Date.now() / 1000 - lastUpdate;
  } else {
    var dt = 0.1;
  }

  var usbPosition = {
    x: newUSBGyroscope[0] / 10,
    y: newUSBGyroscope[1] / 10,
    z: newUSBGyroscope[2] / 10,
  };

  var USBGyroscope = {
    x: ((newUSBGyroscope[3] - 1.3) * Math.PI) / 180,
    z: ((newUSBGyroscope[4] + 0.4) * Math.PI) / 180,
    y: ((newUSBGyroscope[5] - 1.7) * Math.PI) / 180,
  };

  var magnitude = Math.sqrt(
    USBGyroscope.x * USBGyroscope.x +
      USBGyroscope.y * USBGyroscope.y +
      USBGyroscope.z * USBGyroscope.z
  );

  var theta = magnitude * dt;

  USBGyroscope.x /= magnitude;
  USBGyroscope.y /= magnitude;
  USBGyroscope.z /= magnitude;

  var USBQuarternion = new THREE.Quaternion(
    USBGyroscope.x * Math.sin(theta / 2),
    USBGyroscope.y * Math.sin(theta / 2),
    USBGyroscope.z * Math.sin(theta / 2),
    Math.cos(theta / 2)
  );

  USBQuarternion.normalize();

  quaternion.multiplyQuaternions(USBQuarternion, quaternion);
  //quaternion.setFromAxisAngle(new THREE.Vector3(1, 0, 0), Math.PI / 2);

  quaternion.normalize();

  // Apply orientation to seedScene
  seedScene.position.set(usbPosition.x, usbPosition.y, usbPosition.z);
  seedScene.quaternion.copy(quaternion);

  lastUpdate = Date.now() / 1000;
};

// render loop
const onAnimationFrameHandler = (timeStamp) => {
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

// dom
document.body.style.margin = 0;
document.body.appendChild(renderer.domElement);
