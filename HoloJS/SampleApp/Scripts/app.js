var canvas;
var gl;

var cubeVerticesBuffer;
var cubeVerticesTextureCoordBuffer;
var cubeVerticesIndexBuffer;

var cubeImage;
var cubeTexture;

var shaderProgram;
var vertexPositionAttribute;
var textureCoordAttribute;

var pUniform;
var mUniform;
var vUniform;
var samplerUniform;

//
// start
//
// Called when the canvas is created to get the ball rolling.
//
function start() {
    canvas = document.createElement("canvas3D");

    initWebGL(canvas);      // Initialize the GL context

    // Only continue if WebGL is available and working

    if (gl) {
        gl.clearColor(0.0, 0.0, 0.0, 0.0);  // Clear to black, fully opaque
        gl.clearDepth(1.0);                 // Clear everything
        gl.enable(gl.DEPTH_TEST);           // Enable depth testing
        gl.depthFunc(gl.LEQUAL);            // Near things obscure far things

        // Initialize the shaders; this is where all the lighting for the
        // vertices and so forth is established.

        initShaders();

        // Here's where we call the routine that builds all the objects
        // we'll be drawing.

        initBuffers();

        // Next, load and set up the textures we'll be using.

        initTextures();

        // Set up to draw the scene periodically.

        window.requestAnimationFrame(drawScene);
    }
}

start();

//
// initWebGL
//
// Initialize WebGL, returning the GL context or null if
// WebGL isn't available or could not be initialized.
//
function initWebGL() {
    gl = null;

    try {
        gl = canvas.getContext("experimental-webgl");
    }
    catch (e) {
    }

    // If we don't have a GL context, give up now

    if (!gl) {
        alert("Unable to initialize WebGL. Your browser may not support it.");
    }
}

//
// initBuffers
//
// Initialize the buffers we'll need. For this demo, we just have
// one object -- a simple two-dimensional cube.
//
function initBuffers() {

    // Create a buffer for the cube's vertices.

    cubeVerticesBuffer = gl.createBuffer();

    // Select the cubeVerticesBuffer as the one to apply vertex
    // operations to from here out.

    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesBuffer);

    // Now create an array of vertices for the cube.

    var vertices = [
      // Front face
      -0.1, -0.1, 0.1,
       0.1, -0.1, 0.1,
       0.1, 0.1, 0.1,
      -0.1, 0.1, 0.1,

      // Back face
      -0.1, -0.1, -0.1,
      -0.1, 0.1, -0.1,
       0.1, 0.1, -0.1,
       0.1, -0.1, -0.1,

      // Top face
      -0.1, 0.1, -0.1,
      -0.1, 0.1, 0.1,
       0.1, 0.1, 0.1,
       0.1, 0.1, -0.1,

      // Bottom face
      -0.1, -0.1, -0.1,
       0.1, -0.1, -0.1,
       0.1, -0.1, 0.1,
      -0.1, -0.1, 0.1,

      // Right face
       0.1, -0.1, -0.1,
       0.1, 0.1, -0.1,
       0.1, 0.1, 0.1,
       0.1, -0.1, 0.1,

      // Left face
      -0.1, -0.1, -0.1,
      -0.1, -0.1, 0.1,
      -0.1, 0.1, 0.1,
      -0.1, 0.1, -0.1
    ];

    // Now pass the list of vertices into WebGL to build the shape. We
    // do this by creating a Float32Array from the JavaScript array,
    // then use it to fill the current vertex buffer.

    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(vertices), gl.STATIC_DRAW);

    // Map the texture onto the cube's faces.

    cubeVerticesTextureCoordBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesTextureCoordBuffer);

    var textureCoordinates = [
      // Front
      1.0, 1.0,
      0.0, 1.0,
      0.0, 0.0,
      1.0, 0.0,
      // Back
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,
      // Top
      0.0, 1.0,
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      // Bottom
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0,
      // Right
      1.0, 1.0,
      0.0, 1.0,
      0.0, 0.0,
      1.0, 0.0,
      // Left
      0.0, 0.0,
      1.0, 0.0,
      1.0, 1.0,
      0.0, 1.0
    ];

    gl.bufferData(gl.ARRAY_BUFFER, new Float32Array(textureCoordinates),
                  gl.STATIC_DRAW);

    // Build the element array buffer; this specifies the indices
    // into the vertex array for each face's vertices.

    cubeVerticesIndexBuffer = gl.createBuffer();
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer);

    // This array defines each face as two triangles, using the
    // indices into the vertex array to specify each triangle's
    // position.

    var cubeVertexIndices = [
      0,   1,  2,  0,  2,  3,   // front
      4,   5,  6,  4,  6,  7,   // back
      8,   9, 10,  8, 10, 11,   // top
      12, 13, 14, 12, 14, 15,   // bottom
      16, 17, 18, 16, 18, 19,   // right
      20, 21, 22, 20, 22, 23    // left
    ]

    // Now send the element array to GL

    gl.bufferData(gl.ELEMENT_ARRAY_BUFFER,
        new Uint16Array(cubeVertexIndices), gl.STATIC_DRAW);
}

//
// initTextures
//
// Initialize the textures we'll be using, then initiate a load of
// the texture images. The handleTextureLoaded() callback will finish
// the job; it gets called each time a texture finishes loading.
//
function initTextures() {
    cubeTexture = gl.createTexture();
    cubeImage = new Image();
    cubeImage.onload = function () { handleTextureLoaded(cubeImage, cubeTexture); }
    cubeImage.src = "texture.png";
}

function handleTextureLoaded(image, texture) {
    console.log("handleTextureLoaded, image = " + image);
    gl.bindTexture(gl.TEXTURE_2D, texture);
    gl.texImage2D(gl.TEXTURE_2D, 0, gl.RGBA, gl.RGBA,
          gl.UNSIGNED_BYTE, image);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MAG_FILTER, gl.LINEAR);
    gl.texParameteri(gl.TEXTURE_2D, gl.TEXTURE_MIN_FILTER, gl.LINEAR_MIPMAP_NEAREST);
    gl.generateMipmap(gl.TEXTURE_2D);
}

//
// drawScene
//
// Draw the scene.
//
function drawScene() {
    // Clear the canvas before we start drawing on it.
    gl.clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT);
    
    // Identity perspective transform.
    var identity = [
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    ]

    // Set up a simple translation transform.
    var model = [
        1.0, 0.0, 0.0, 0.0,
        0.0, 1.0, 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
       -0.0, 0.0,-1.5, 1.0
       ]

    // Draw the cube by binding the array buffer to the cube's vertices
    // array, setting attributes, and pushing it to GL.
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesBuffer);
    gl.vertexAttribPointer(vertexPositionAttribute, 3, gl.FLOAT, false, 0, 0);

    // Set the texture coordinates attribute for the vertices.
    gl.bindBuffer(gl.ARRAY_BUFFER, cubeVerticesTextureCoordBuffer);
    gl.vertexAttribPointer(textureCoordAttribute, 2, gl.FLOAT, false, 0, 0);

    // Specify the texture to map onto the faces.
    gl.activeTexture(gl.TEXTURE0);
    gl.bindTexture(gl.TEXTURE_2D, cubeTexture);
    gl.uniform1i(samplerUniform, 0);

    // Draw the cube.
    gl.bindBuffer(gl.ELEMENT_ARRAY_BUFFER, cubeVerticesIndexBuffer);
    gl.uniformMatrix4fv(pUniform, false, new Float32Array(identity));
    gl.uniformMatrix4fv(mUniform, false, new Float32Array(model));
    gl.uniformMatrix4fv(vUniform, false, window.getViewMatrix());
    gl.drawElements(gl.TRIANGLES, 36, gl.UNSIGNED_SHORT, 0);

    // Present the frame.
    window.requestAnimationFrame(drawScene);
}

//
// initShaders
//
// Initialize the shaders, so WebGL knows how to light our scene.
//
function initShaders() {
    var fragmentShader = getShader(gl, "shader-fs");
    var vertexShader = getShader(gl, "shader-vs");

    // Create the shader program

    shaderProgram = gl.createProgram();
    gl.attachShader(shaderProgram, vertexShader);
    gl.attachShader(shaderProgram, fragmentShader);
    gl.linkProgram(shaderProgram);

    // If creating the shader program failed, alert

    if (!gl.getProgramParameter(shaderProgram, gl.LINK_STATUS)) {
        alert("Unable to initialize the shader program: " + gl.getProgramInfoLog(shader));
    }

    gl.useProgram(shaderProgram);

    vertexPositionAttribute = gl.getAttribLocation(shaderProgram, "aVertexPosition");
    gl.enableVertexAttribArray(vertexPositionAttribute);

    textureCoordAttribute = gl.getAttribLocation(shaderProgram, "aTextureCoord");
    gl.enableVertexAttribArray(textureCoordAttribute);

    pUniform = gl.getUniformLocation(shaderProgram, "uPMatrix");
    mUniform = gl.getUniformLocation(shaderProgram, "uMMatrix");
    vUniform = gl.getUniformLocation(shaderProgram, "uVMatrix");
    samplerUniform = gl.getUniformLocation(shaderProgram, "uSampler");
}

//
// getShader
//
// Loads a shader program by scouring the current document,
// looking for a script with the specified ID.
//
function getShader(gl, id) {
    var vs = "attribute vec3 aVertexPosition;\
    attribute vec2 aTextureCoord;\
    uniform mat4 uMMatrix;\
    uniform mat4 uVMatrix;\
    uniform mat4 uPMatrix;\
    varying highp vec2 vTextureCoord;\
    void main(void) {\
        gl_Position = uPMatrix * uVMatrix * uMMatrix * vec4(aVertexPosition, 1.0);\
        vTextureCoord = aTextureCoord;\
    }";

    var fs = "varying highp vec2 vTextureCoord;\
    uniform sampler2D uSampler;\
    void main(void) {\
        gl_FragColor = texture2D(uSampler, vec2(vTextureCoord.s, vTextureCoord.t));\
    }";

    var shaderScript;
    var shader;
    if (id === "shader-fs") {
        shaderScript = fs;
        console.log(fs);
        shader = gl.createShader(gl.FRAGMENT_SHADER);
    } else if (id === "shader-vs") {
        shaderScript = vs;
        shader = gl.createShader(gl.VERTEX_SHADER);
    }
    else {
        return null;
    }

    // Send the source to the shader object

    gl.shaderSource(shader, shaderScript);

    // Compile the shader program

    gl.compileShader(shader);

    // See if it compiled successfully

    if (!gl.getShaderParameter(shader, gl.COMPILE_STATUS)) {
        console.log("An error occurred compiling the shaders: " + gl.getShaderInfoLog(shader));
        return null;
    }

    return shader;
}
