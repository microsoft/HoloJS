(function () {
    
    nativeInterface.makeWebGLRenderingContext = function() {

        this.glContext = nativeInterface.webgl.createContext();

        this.getError = function () {
            return 0;
        };

        this.getShaderPrecisionFormat = function (shadertype, precisiontype) {
            return nativeInterface.webgl.getShaderPrecisionFormat(this.glContext, shadertype, precisiontype);
        };
        this.getExtension = function (name) {
            return nativeInterface.webgl.getExtension(this.glContext, name);
        };
        this.getParameter = function (pname) {
            return nativeInterface.webgl.getParameter(this.glContext, pname);
        };
        this.createRenderbuffer = function () {
            return nativeInterface.webgl.createRenderbuffer(this.glContext);
        };
        this.bindRenderbuffer = function (target, renderbuffer) {
            if (renderbuffer) {
                return nativeInterface.webgl.bindRenderbuffer(this.glContext, target, renderbuffer.external);
            } else {
                return nativeInterface.webgl.bindRenderbuffer(this.glContext, target, null);
            }
        };
        this.renderbufferStorage = function (target, internalformat, width, height) {
            return nativeInterface.webgl.renderbufferStorage(this.glContext, target, internalformat, width, height);
        };
        this.createFramebuffer = function () {
            return nativeInterface.webgl.createFramebuffer(this.glContext);
        };
        this.bindFramebuffer = function (target, framebuffer) {
            if (framebuffer) {
                return nativeInterface.webgl.bindFramebuffer(this.glContext, target, framebuffer.external);
            } else {
                return nativeInterface.webgl.bindFramebuffer(this.glContext, target, null);
            }
        };
        this.framebufferRenderbuffer = function (target, attachment, renderbuffertarget, renderbuffer) {
            return nativeInterface.webgl.framebufferRenderbuffer(this.glContext, target, attachment, renderbuffertarget, renderbuffer.external);
        };
        this.framebufferTexture2D = function (target, attachment, textarget, texture, level) {
            return nativeInterface.webgl.framebufferTexture2D(this.glContext, target, attachment, textarget, texture.external, level);
        };
        this.createTexture = function () {
            return nativeInterface.webgl.createTexture(this.glContext);
        };
        this.bindTexture = function (target, texture) {
            if (texture) {
                return nativeInterface.webgl.bindTexture(this.glContext, target, texture.external);
            } else {
                return nativeInterface.webgl.bindTexture(this.glContext, target, null);
            }
        };
        this.texParameteri = function (target, pname, param) {
            return nativeInterface.webgl.texParameteri(this.glContext, target, pname, param);
        };

        this.texImage2D = function (target, level, internalformat, width, height, border, format, type, pixels) {
            return nativeInterface.webgl.texImage2D(this.glContext, target, level, internalformat, width, height, border, format, type, pixels.external);
        };

        this.texImage2D = function () {
            if (arguments.length === 9) {
                //GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, ArrayBufferView? pixels
                nativeInterface.webgl.texImage2D1(this.glContext, arguments[0], arguments[1], arguments[2], arguments[3], arguments[4], arguments[5], arguments[6], arguments[7], arguments[8]);
            } else if (arguments.length === 6 && (arguments[5] instanceof Image)) {
                var image = arguments[5];
                //GLenum target, GLint level, GLenum internalformat, GLenum format, GLenum type, HTMLImageElement image
                nativeInterface.webgl.texImage2D2(this.glContext, arguments[0], arguments[1], arguments[2], image.width, image.height, arguments[3], arguments[4], image.native);
            } else if ((arguments.length === 6) && (arguments[5].isCanvas2D === true) && arguments[5].context && arguments[5].context.context2d) {
                //GLenum target, GLint level, GLenum internalformat, GLenum format, GLenum type, canvas
                var canvas = arguments[5];
                nativeInterface.webgl.texImage2D3(this.glContext, arguments[0], arguments[1], arguments[2], canvas.width, canvas.height, arguments[3], arguments[4], canvas.context.context2d);
            } else if (arguments.length === 6 && (arguments[5] instanceof HTMLVideoElement)) {
                var video = arguments[5];
                nativeInterface.webgl.texImage2D4(this.glContext, arguments[0], arguments[1], arguments[2], video.width, video.height, arguments[3], arguments[4], video.native);
            } else {
                throw DOMException();
            }
        };


        this.activeTexture = function (texture) {
            return nativeInterface.webgl.activeTexture(this.glContext, texture);
        };
        this.generateMipmap = function (target) {
            return nativeInterface.webgl.generateMipmap(this.glContext, target);
        };
        this.pixelStorei = function (pname, param) {
            return nativeInterface.webgl.pixelStorei(this.glContext, pname, param);
        };
        this.clearDepth = function (depth) {
            return nativeInterface.webgl.clearDepth(this.glContext, depth);
        };
        this.clearStencil = function (s) {
            return nativeInterface.webgl.clearStencil(this.glContext, s);
        };
        this.enable = function (cap) {
            return nativeInterface.webgl.enable(this.glContext, cap);
        };
        this.disable = function (cap) {
            return nativeInterface.webgl.disable(this.glContext, cap);
        };
        this.depthFunc = function (func) {
            return nativeInterface.webgl.depthFunc(this.glContext, func);
        };
        this.depthMask = function (flag) {
            return nativeInterface.webgl.depthMask(this.glContext, flag);
        };
        this.depthRange = function (zNear, zFar) {
            return nativeInterface.webgl.depthRange(this.glContext, zNear, zFar);
        };
        this.frontFace = function (mode) {
            return nativeInterface.webgl.frontFace(this.glContext, mode);
        };
        this.cullFace = function (mode) {
            return nativeInterface.webgl.cullFace(this.glContext, mode);
        };
        this.blendColor = function (red, green, blue, alpha) {
            return nativeInterface.webgl.blendColor(this.glContext, red, green, blue, alpha);
        };
        this.blendEquation = function (mode) {
            return nativeInterface.webgl.blendEquation(this.glContext, mode);
        };
        this.blendEquationSeparate = function (modeRGB, modeAlpha) {
            return nativeInterface.webgl.blendEquationSeparate(this.glContext, modeRGB, modeAlpha);
        };
        this.blendFunc = function (sfactor, dfactor) {
            return nativeInterface.webgl.blendFunc(this.glContext, sfactor, dfactor);
        };
        this.blendFuncSeparate = function (srcRGB, dstRGB, srcAlpha, dstAlpha) {
            return nativeInterface.webgl.blendFuncSeparate(this.glContext, srcRGB, dstRGB, srcAlpha, dstAlpha);
        };
        this.scissor = function (x, y, width, height) {
            return nativeInterface.webgl.scissor(this.glContext, x, y, width, height);
        };
        this.viewport = function (x, y, width, height) {
            return nativeInterface.webgl.viewport(this.glContext, x, y, width, height);
        };
        this.clearColor = function (red, green, blue, alpha) {
            return nativeInterface.webgl.clearColor(this.glContext, red, green, blue, alpha);
        };
        this.clear = function (mask) {
            return nativeInterface.webgl.clear(this.glContext, mask);
        };
        this.createBuffer = function () {
            return nativeInterface.webgl.createBuffer(this.glContext);
        };
        this.bindBuffer = function (target, buffer) {
            if (buffer) {
                return nativeInterface.webgl.bindBuffer(this.glContext, target, buffer.external);
            } else {
                return nativeInterface.webgl.bindBuffer(this.glContext, target, null);
            }
        };

        this.bufferData = function (target, size, usage) {
            if (typeof size === "number") {
                return nativeInterface.webgl.bufferData1(this.glContext, target, size, usage);
            } else {
                // size is data
                return nativeInterface.webgl.bufferData2(this.glContext, target, size, usage);
            }
        };

        this.deleteBuffer = function (buffer) {
            return nativeInterface.webgl.deleteBuffer(this.glContext, buffer.external);
        };

        this.colorMask = function (red, green, blue, alpha) {
            return nativeInterface.webgl.colorMask(this.glContext, red, green, blue, alpha);
        };
        this.drawArrays = function (mode, first, count) {
            return nativeInterface.webgl.drawArrays(this.glContext, mode, first, count);
        };
        this.drawElements = function (mode, count, type, offset) {
            return nativeInterface.webgl.drawElements(this.glContext, mode, count, type, offset);
        };
        this.enableVertexAttribArray = function (index) {
            return nativeInterface.webgl.enableVertexAttribArray(this.glContext, index);
        };
        this.disableVertexAttribArray = function (index) {
            return nativeInterface.webgl.disableVertexAttribArray(this.glContext, index);
        };
        this.vertexAttribPointer = function (indx, size, type, normalized, stride, offset) {
            return nativeInterface.webgl.vertexAttribPointer(this.glContext, indx, size, type, normalized, stride, offset);
        };
        this.createProgram = function () {
            return nativeInterface.webgl.createProgram(this.glContext);
        };
        this.useProgram = function (program) {
            return nativeInterface.webgl.useProgram(this.glContext, program.external);
        };
        this.validateProgram = function (program) {
            return nativeInterface.webgl.validateProgram(this.glContext, program.external);
        };
        this.linkProgram = function (program) {
            return nativeInterface.webgl.linkProgram(this.glContext, program.external);
        };
        this.getProgramParameter = function (program, pname) {
            return nativeInterface.webgl.getProgramParameter(this.glContext, program.external, pname);
        };
        this.getProgramInfoLog = function (program) {
            return nativeInterface.webgl.getProgramInfoLog(this.glContext, program.external);
        };
        this.deleteProgram = function (program) {
            return nativeInterface.webgl.deleteProgram(this.glContext, program.external);
        };
        this.bindAttribLocation = function (program, index, name) {
            return nativeInterface.webgl.bindAttribLocation(this.glContext, program.external, index, name);
        };
        this.getActiveUniform = function (program, index) {
            return nativeInterface.webgl.getActiveUniform(this.glContext, program.external, index);
        };
        this.getActiveAttrib = function (program, index) {
            return nativeInterface.webgl.getActiveAttrib(this.glContext, program.external, index);
        };
        this.getAttribLocation = function (program, name) {
            return nativeInterface.webgl.getAttribLocation(this.glContext, program.external, name);
        };
        this.createShader = function (type) {
            return nativeInterface.webgl.createShader(this.glContext, type);
        };
        this.shaderSource = function (shader, source) {
            return nativeInterface.webgl.shaderSource(this.glContext, shader.external, source);
        };
        this.compileShader = function (shader) {
            return nativeInterface.webgl.compileShader(this.glContext, shader.external);
        };
        this.getShaderParameter = function (shader, pname) {
            return nativeInterface.webgl.getShaderParameter(this.glContext, shader.external, pname);
        };
        this.getShaderInfoLog = function (shader) {
            return nativeInterface.webgl.getShaderInfoLog(this.glContext, shader.external);
        };
        this.attachShader = function (program, shader) {
            return nativeInterface.webgl.attachShader(this.glContext, program.external, shader.external);
        };
        this.deleteShader = function (shader) {
            return nativeInterface.webgl.deleteShader(this.glContext, shader.external);
        };
        this.getUniformLocation = function (program, name) {
            return nativeInterface.webgl.getUniformLocation(this.glContext, program.external, name);
        };
        this.uniform1f = function (location, x) {
            return nativeInterface.webgl.uniform1f(this.glContext, location.external, x);
        };
        this.uniform1fv = function (location, v) {
            return nativeInterface.webgl.uniform1fv(this.glContext, location.external, v);
        };
        this.uniform1i = function (location, x) {
            return nativeInterface.webgl.uniform1i(this.glContext, location.external, x);
        };
        this.uniform1iv = function (location, v) {
            return nativeInterface.webgl.uniform1iv(this.glContext, location.external, v);
        };
        this.uniform2f = function (location, x, y) {
            return nativeInterface.webgl.uniform2f(this.glContext, location.external, x, y);
        };
        this.uniform2fv = function (location, v) {
            return nativeInterface.webgl.uniform2fv(this.glContext, location.external, v);
        };
        this.uniform2i = function (location, x, y) {
            return nativeInterface.webgl.uniform2i(this.glContext, location.external, x, y);
        };
        this.uniform2iv = function (location, v) {
            return nativeInterface.webgl.uniform2iv(this.glContext, location.external, v);
        };
        this.uniform3f = function (location, x, y, z) {
            return nativeInterface.webgl.uniform3f(this.glContext, location.external, x, y, z);
        };
        this.uniform3fv = function (location, v) {
            return nativeInterface.webgl.uniform3fv(this.glContext, location.external, v);
        };
        this.uniform3i = function (location, x, y, z) {
            return nativeInterface.webgl.uniform3i(this.glContext, location.external, x, y, z);
        };
        this.uniform3iv = function (location, v) {
            return nativeInterface.webgl.uniform3iv(this.glContext, location.external, v);
        };
        this.uniform4f = function (location, x, y, z, w) {
            return nativeInterface.webgl.uniform4f(this.glContext, location.external, x, y, z, w);
        };
        this.uniform4fv = function (location, v) {
            return nativeInterface.webgl.uniform4fv(this.glContext, location.external, v);
        };
        this.uniform4i = function (location, x, y, z, w) {
            return nativeInterface.webgl.uniform4i(this.glContext, location.external, x, y, z, w);
        };
        this.uniform4iv = function (location, v) {
            return nativeInterface.webgl.uniform4iv(this.glContext, location.external, v);
        };
        this.uniformMatrix2fv = function (location, transpose, value) {
            return nativeInterface.webgl.uniformMatrix2fv(this.glContext, location.external, transpose, value);
        };
        this.uniformMatrix3fv = function (location, transpose, value) {
            return nativeInterface.webgl.uniformMatrix3fv(this.glContext, location.external, transpose, value);
        };
        this.uniformMatrix4fv = function (location, transpose, value) {
            return nativeInterface.webgl.uniformMatrix4fv(this.glContext, location.external, transpose, value);
        };
        this.stencilFunc = function (func, ref, mask) {
            return nativeInterface.webgl.stencilFunc(this.glContext, func, ref, mask);
        };
        this.stencilMask = function (mask) {
            return nativeInterface.webgl.stencilMask(this.glContext, mask);
        };
        this.stencilOp = function (fail, zfail, zpass) {
            return nativeInterface.webgl.stencilOp(this.glContext, fail, zfail, zpass);
        };

        this.DEPTH_BUFFER_BIT = 0x00000100;
        this.STENCIL_BUFFER_BIT = 0x00000400;
        this.COLOR_BUFFER_BIT = 0x00004000;

        /* BeginMode */
        this.POINTS = 0x0000;
        this.LINES = 0x0001;
        this.LINE_LOOP = 0x0002;
        this.LINE_STRIP = 0x0003;
        this.TRIANGLES = 0x0004;
        this.TRIANGLE_STRIP = 0x0005;
        this.TRIANGLE_FAN = 0x0006;

        /* BlendingFactorDest */
        this.ZERO = 0;
        this.ONE = 1;
        this.SRC_COLOR = 0x0300;
        this.ONE_MINUS_SRC_COLOR = 0x0301;
        this.SRC_ALPHA = 0x0302;
        this.ONE_MINUS_SRC_ALPHA = 0x0303;
        this.DST_ALPHA = 0x0304;
        this.ONE_MINUS_DST_ALPHA = 0x0305;

        /* BlendingFactorSrc */
        this.DST_COLOR = 0x0306;
        this.ONE_MINUS_DST_COLOR = 0x0307;
        this.SRC_ALPHA_SATURATE = 0x0308;

        /* BlendEquationSeparate */
        this.FUNC_ADD = 0x8006;
        this.BLEND_EQUATION = 0x8009;
        this.BLEND_EQUATION_RGB = 0x8009;   /* same as BLEND_EQUATION */
        this.BLEND_EQUATION_ALPHA = 0x883D;

        /* BlendSubtract */
        this.FUNC_SUBTRACT = 0x800A;
        this.FUNC_REVERSE_SUBTRACT = 0x800B;

        /* Separate Blend Functions */
        this.BLEND_DST_RGB = 0x80C8;
        this.BLEND_SRC_RGB = 0x80C9;
        this.BLEND_DST_ALPHA = 0x80CA;
        this.BLEND_SRC_ALPHA = 0x80CB;
        this.CONSTANT_COLOR = 0x8001;
        this.ONE_MINUS_CONSTANT_COLOR = 0x8002;
        this.CONSTANT_ALPHA = 0x8003;
        this.ONE_MINUS_CONSTANT_ALPHA = 0x8004;
        this.BLEND_COLOR = 0x8005;

        /* Buffer Objects */
        this.ARRAY_BUFFER = 0x8892;
        this.ELEMENT_ARRAY_BUFFER = 0x8893;
        this.ARRAY_BUFFER_BINDING = 0x8894;
        this.ELEMENT_ARRAY_BUFFER_BINDING = 0x8895;

        this.STREAM_DRAW = 0x88E0;
        this.STATIC_DRAW = 0x88E4;
        this.DYNAMIC_DRAW = 0x88E8;

        this.BUFFER_SIZE = 0x8764;
        this.BUFFER_USAGE = 0x8765;

        this.CURRENT_VERTEX_ATTRIB = 0x8626;

        /* CullFaceMode */
        this.FRONT = 0x0404;
        this.BACK = 0x0405;
        this.FRONT_AND_BACK = 0x0408;

        /* EnableCap */
        /* TEXTURE_2D */
        this.CULL_FACE = 0x0B44;
        this.BLEND = 0x0BE2;
        this.DITHER = 0x0BD0;
        this.STENCIL_TEST = 0x0B90;
        this.DEPTH_TEST = 0x0B71;
        this.SCISSOR_TEST = 0x0C11;
        this.POLYGON_OFFSET_FILL = 0x8037;
        this.SAMPLE_ALPHA_TO_COVERAGE = 0x809E;
        this.SAMPLE_COVERAGE = 0x80A0;

        /* ErrorCode */

        this.NO_ERROR = 0;
        this.INVALID_ENUM = 0x0500;
        this.INVALID_VALUE = 0x0501;
        this.INVALID_OPERATION = 0x0502;
        this.OUT_OF_MEMORY = 0x0505;

        /* FrontFaceDirection */
        this.CW = 0x0900;
        this.CCW = 0x0901;

        /* GetPName */
        this.LINE_WIDTH = 0x0B21;
        this.ALIASED_POINT_SIZE_RANGE = 0x846D;
        this.ALIASED_LINE_WIDTH_RANGE = 0x846E;
        this.CULL_FACE_MODE = 0x0B45;
        this.FRONT_FACE = 0x0B46;
        this.DEPTH_RANGE = 0x0B70;
        this.DEPTH_WRITEMASK = 0x0B72;
        this.DEPTH_CLEAR_VALUE = 0x0B73;
        this.DEPTH_FUNC = 0x0B74;
        this.STENCIL_CLEAR_VALUE = 0x0B91;
        this.STENCIL_FUNC = 0x0B92;
        this.STENCIL_FAIL = 0x0B94;
        this.STENCIL_PASS_DEPTH_FAIL = 0x0B95;
        this.STENCIL_PASS_DEPTH_PASS = 0x0B96;
        this.STENCIL_REF = 0x0B97;
        this.STENCIL_VALUE_MASK = 0x0B93;
        this.STENCIL_WRITEMASK = 0x0B98;
        this.STENCIL_BACK_FUNC = 0x8800;
        this.STENCIL_BACK_FAIL = 0x8801;
        this.STENCIL_BACK_PASS_DEPTH_FAIL = 0x8802;
        this.STENCIL_BACK_PASS_DEPTH_PASS = 0x8803;
        this.STENCIL_BACK_REF = 0x8CA3;
        this.STENCIL_BACK_VALUE_MASK = 0x8CA4;
        this.STENCIL_BACK_WRITEMASK = 0x8CA5;
        this.VIEWPORT = 0x0BA2;
        this.SCISSOR_BOX = 0x0C10;
        /*      SCISSOR_TEST */
        this.COLOR_CLEAR_VALUE = 0x0C22;
        this.COLOR_WRITEMASK = 0x0C23;
        this.UNPACK_ALIGNMENT = 0x0CF5;
        this.PACK_ALIGNMENT = 0x0D05;
        this.MAX_TEXTURE_SIZE = 0x0D33;
        this.MAX_VIEWPORT_DIMS = 0x0D3A;
        this.SUBPIXEL_BITS = 0x0D50;
        this.RED_BITS = 0x0D52;
        this.GREEN_BITS = 0x0D53;
        this.BLUE_BITS = 0x0D54;
        this.ALPHA_BITS = 0x0D55;
        this.DEPTH_BITS = 0x0D56;
        this.STENCIL_BITS = 0x0D57;
        this.POLYGON_OFFSET_UNITS = 0x2A00;
        /*      POLYGON_OFFSET_FILL */
        this.POLYGON_OFFSET_FACTOR = 0x8038;
        this.TEXTURE_BINDING_2D = 0x8069;
        this.SAMPLE_BUFFERS = 0x80A8;
        this.SAMPLES = 0x80A9;
        this.SAMPLE_COVERAGE_VALUE = 0x80AA;
        SAMPLE_COVERAGE_INVERT = 0x80AB;

        /* GetTextureParameter */
        /*      TEXTURE_MAG_FILTER */
        /*      TEXTURE_MIN_FILTER */
        /*      TEXTURE_WRAP_S */
        /*      TEXTURE_WRAP_T */

        this.COMPRESSED_TEXTURE_FORMATS = 0x86A3;

        /* HintMode */
        this.DONT_CARE = 0x1100;
        this.FASTEST = 0x1101;
        this.NICEST = 0x1102;

        /* HintTarget */
        this.GENERATE_MIPMAP_HINT = 0x8192;

        /* DataType */
        this.BYTE = 0x1400;
        this.UNSIGNED_BYTE = 0x1401;
        this.SHORT = 0x1402;
        this.UNSIGNED_SHORT = 0x1403;
        this.INT = 0x1404;
        this.UNSIGNED_INT = 0x1405;
        this.FLOAT = 0x1406;

        /* PixelFormat */
        this.DEPTH_COMPONENT = 0x1902;
        this.ALPHA = 0x1906;
        this.RGB = 0x1907;
        this.RGBA = 0x1908;
        this.LUMINANCE = 0x1909;
        this.LUMINANCE_ALPHA = 0x190A;

        /* PixelType */
        /*      UNSIGNED_BYTE */
        this.UNSIGNED_SHORT_4_4_4_4 = 0x8033;
        this.UNSIGNED_SHORT_5_5_5_1 = 0x8034;
        this.UNSIGNED_SHORT_5_6_5 = 0x8363;

        /* Shaders */
        this.FRAGMENT_SHADER = 0x8B30;
        this.VERTEX_SHADER = 0x8B31;
        this.MAX_VERTEX_ATTRIBS = 0x8869;
        this.MAX_VERTEX_UNIFORM_VECTORS = 0x8DFB;
        this.MAX_VARYING_VECTORS = 0x8DFC;
        this.MAX_COMBINED_TEXTURE_IMAGE_UNITS = 0x8B4D;
        this.MAX_VERTEX_TEXTURE_IMAGE_UNITS = 0x8B4C;
        this.MAX_TEXTURE_IMAGE_UNITS = 0x8872;
        this.MAX_FRAGMENT_UNIFORM_VECTORS = 0x8DFD;
        this.SHADER_TYPE = 0x8B4F;
        this.DELETE_STATUS = 0x8B80;
        this.LINK_STATUS = 0x8B82;
        this.VALIDATE_STATUS = 0x8B83;
        this.ATTACHED_SHADERS = 0x8B85;
        this.ACTIVE_UNIFORMS = 0x8B86;
        this.ACTIVE_ATTRIBUTES = 0x8B89;
        this.SHADING_LANGUAGE_VERSION = 0x8B8C;
        this.CURRENT_PROGRAM = 0x8B8D;

        /* StencilFunction */
        this.NEVER = 0x0200;
        this.LESS = 0x0201;
        this.EQUAL = 0x0202;
        this.LEQUAL = 0x0203;
        this.GREATER = 0x0204;
        this.NOTEQUAL = 0x0205;
        this.GEQUAL = 0x0206;
        this.ALWAYS = 0x0207;

        /* StencilOp */
        /*      ZERO */
        this.KEEP = 0x1E00;
        this.REPLACE = 0x1E01;
        this.INCR = 0x1E02;
        this.DECR = 0x1E03;
        this.INVERT = 0x150A;
        this.INCR_WRAP = 0x8507;
        this.DECR_WRAP = 0x8508;

        /* StringName */
        this.VENDOR = 0x1F00;
        this.RENDERER = 0x1F01;
        this.VERSION = 0x1F02;

        /* TextureMagFilter */
        this.NEAREST = 0x2600;
        this.LINEAR = 0x2601;

        /* TextureMinFilter */
        /*      NEAREST */
        /*      LINEAR */
        this.NEAREST_MIPMAP_NEAREST = 0x2700;
        this.LINEAR_MIPMAP_NEAREST = 0x2701;
        this.NEAREST_MIPMAP_LINEAR = 0x2702;
        this.LINEAR_MIPMAP_LINEAR = 0x2703;

        /* TextureParameterName */
        this.TEXTURE_MAG_FILTER = 0x2800;
        this.TEXTURE_MIN_FILTER = 0x2801;
        this.TEXTURE_WRAP_S = 0x2802;
        this.TEXTURE_WRAP_T = 0x2803;

        /* TextureTarget */
        this.TEXTURE_2D = 0x0DE1;
        this.TEXTURE = 0x1702;

        this.TEXTURE_CUBE_MAP = 0x8513;
        this.TEXTURE_BINDING_CUBE_MAP = 0x8514;
        this.TEXTURE_CUBE_MAP_POSITIVE_X = 0x8515;
        this.TEXTURE_CUBE_MAP_NEGATIVE_X = 0x8516;
        this.TEXTURE_CUBE_MAP_POSITIVE_Y = 0x8517;
        this.TEXTURE_CUBE_MAP_NEGATIVE_Y = 0x8518;
        this.TEXTURE_CUBE_MAP_POSITIVE_Z = 0x8519;
        this.TEXTURE_CUBE_MAP_NEGATIVE_Z = 0x851A;
        this.MAX_CUBE_MAP_TEXTURE_SIZE = 0x851C;

        /* TextureUnit */
        this.TEXTURE0 = 0x84C0;
        this.TEXTURE1 = 0x84C1;
        this.TEXTURE2 = 0x84C2;
        this.TEXTURE3 = 0x84C3;
        this.TEXTURE4 = 0x84C4;
        this.TEXTURE5 = 0x84C5;
        this.TEXTURE6 = 0x84C6;
        this.TEXTURE7 = 0x84C7;
        this.TEXTURE8 = 0x84C8;
        this.TEXTURE9 = 0x84C9;
        this.TEXTURE10 = 0x84CA;
        this.TEXTURE11 = 0x84CB;
        this.TEXTURE12 = 0x84CC;
        this.TEXTURE13 = 0x84CD;
        this.TEXTURE14 = 0x84CE;
        this.TEXTURE15 = 0x84CF;
        this.TEXTURE16 = 0x84D0;
        this.TEXTURE17 = 0x84D1;
        this.TEXTURE18 = 0x84D2;
        this.TEXTURE19 = 0x84D3;
        this.TEXTURE20 = 0x84D4;
        this.TEXTURE21 = 0x84D5;
        this.TEXTURE22 = 0x84D6;
        this.TEXTURE23 = 0x84D7;
        this.TEXTURE24 = 0x84D8;
        this.TEXTURE25 = 0x84D9;
        this.TEXTURE26 = 0x84DA;
        this.TEXTURE27 = 0x84DB;
        this.TEXTURE28 = 0x84DC;
        this.TEXTURE29 = 0x84DD;
        this.TEXTURE30 = 0x84DE;
        this.TEXTURE31 = 0x84DF;
        this.ACTIVE_TEXTURE = 0x84E0;

        /* TextureWrapMode */
        this.REPEAT = 0x2901;
        this.CLAMP_TO_EDGE = 0x812F;
        this.MIRRORED_REPEAT = 0x8370;

        /* Uniform Types */
        this.FLOAT_VEC2 = 0x8B50;
        this.FLOAT_VEC3 = 0x8B51;
        this.FLOAT_VEC4 = 0x8B52;
        this.INT_VEC2 = 0x8B53;
        this.INT_VEC3 = 0x8B54;
        this.INT_VEC4 = 0x8B55;
        this.BOOL = 0x8B56;
        this.BOOL_VEC2 = 0x8B57;
        this.BOOL_VEC3 = 0x8B58;
        this.BOOL_VEC4 = 0x8B59;
        this.FLOAT_MAT2 = 0x8B5A;
        this.FLOAT_MAT3 = 0x8B5B;
        this.FLOAT_MAT4 = 0x8B5C;
        this.SAMPLER_2D = 0x8B5E;
        this.SAMPLER_CUBE = 0x8B60;

        /* Vertex Arrays */
        this.VERTEX_ATTRIB_ARRAY_ENABLED = 0x8622;
        this.VERTEX_ATTRIB_ARRAY_SIZE = 0x8623;
        this.VERTEX_ATTRIB_ARRAY_STRIDE = 0x8624;
        this.VERTEX_ATTRIB_ARRAY_TYPE = 0x8625;
        this.VERTEX_ATTRIB_ARRAY_NORMALIZED = 0x886A;
        this.VERTEX_ATTRIB_ARRAY_POINTER = 0x8645;
        this.VERTEX_ATTRIB_ARRAY_BUFFER_BINDING = 0x889F;

        /* Read Format */
        this.IMPLEMENTATION_COLOR_READ_TYPE = 0x8B9A;
        this.IMPLEMENTATION_COLOR_READ_FORMAT = 0x8B9B;

        /* Shader Source */
        this.COMPILE_STATUS = 0x8B81;

        /* Shader Precision-Specified Types */
        this.LOW_FLOAT = 0x8DF0;
        this.MEDIUM_FLOAT = 0x8DF1;
        this.HIGH_FLOAT = 0x8DF2;
        this.LOW_INT = 0x8DF3;
        this.MEDIUM_INT = 0x8DF4;
        this.HIGH_INT = 0x8DF5;

        /* Framebuffer Object. */
        this.FRAMEBUFFER = 0x8D40;
        this.RENDERBUFFER = 0x8D41;

        this.RGBA4 = 0x8056;
        this.RGB5_A1 = 0x8057;
        this.RGB565 = 0x8D62;
        this.DEPTH_COMPONENT16 = 0x81A5;
        this.STENCIL_INDEX = 0x1901;
        this.STENCIL_INDEX8 = 0x8D48;
        this.DEPTH_STENCIL = 0x84F9;

        this.RENDERBUFFER_WIDTH = 0x8D42;
        this.RENDERBUFFER_HEIGHT = 0x8D43;
        this.RENDERBUFFER_INTERNAL_FORMAT = 0x8D44;
        this.RENDERBUFFER_RED_SIZE = 0x8D50;
        this.RENDERBUFFER_GREEN_SIZE = 0x8D51;
        this.RENDERBUFFER_BLUE_SIZE = 0x8D52;
        this.RENDERBUFFER_ALPHA_SIZE = 0x8D53;
        this.RENDERBUFFER_DEPTH_SIZE = 0x8D54;
        this.RENDERBUFFER_STENCIL_SIZE = 0x8D55;

        this.FRAMEBUFFER_ATTACHMENT_OBJECT_TYPE = 0x8CD0;
        this.FRAMEBUFFER_ATTACHMENT_OBJECT_NAME = 0x8CD1;
        this.FRAMEBUFFER_ATTACHMENT_TEXTURE_LEVEL = 0x8CD2;
        this.FRAMEBUFFER_ATTACHMENT_TEXTURE_CUBE_MAP_FACE = 0x8CD3;

        this.COLOR_ATTACHMENT0 = 0x8CE0;
        this.DEPTH_ATTACHMENT = 0x8D00;
        this.STENCIL_ATTACHMENT = 0x8D20;
        this.DEPTH_STENCIL_ATTACHMENT = 0x821A;

        this.NONE = 0;

        this.FRAMEBUFFER_COMPLETE = 0x8CD5;
        this.FRAMEBUFFER_INCOMPLETE_ATTACHMENT = 0x8CD6;
        this.FRAMEBUFFER_INCOMPLETE_MISSING_ATTACHMENT = 0x8CD7;
        this.FRAMEBUFFER_INCOMPLETE_DIMENSIONS = 0x8CD9;
        this.FRAMEBUFFER_UNSUPPORTED = 0x8CDD;

        this.FRAMEBUFFER_BINDING = 0x8CA6;
        this.RENDERBUFFER_BINDING = 0x8CA7;
        this.MAX_RENDERBUFFER_SIZE = 0x84E8;

        this.INVALID_FRAMEBUFFER_OPERATION = 0x0506;

        /* WebGL-specific enums */
        this.UNPACK_FLIP_Y_WEBGL = 0x9240;
        this.UNPACK_PREMULTIPLY_ALPHA_WEBGL = 0x9241;
        this.CONTEXT_LOST_WEBGL = 0x9242;
        this.UNPACK_COLORSPACE_CONVERSION_WEBGL = 0x9243;
        this.BROWSER_DEFAULT_WEBGL = 0x9244;
    }
})();

