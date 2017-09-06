(function (global, factory) {
    typeof exports === 'object' && typeof module !== 'undefined' ? factory(exports, require('three')) :
        typeof define === 'function' && define.amd ? define(['exports', 'three'], factory) :
            (factory((global.THREE_HOLO = {}), global.THREE));
}(this, (function (exports, three) {
    'use strict';

    for (let chunk in three.ShaderChunk) {
        three.ShaderChunk[chunk] = three.ShaderChunk[chunk].replace('transpose', 'transpose2');
    }

    class HolographicEffect {
        constructor(renderer) {
            if (window.experimentalHolographic === true) {
                console.log(`THREE_HOLO.HolographicEffect`);
                console.log(`Auto Stereo Rendering: [${window.holographicSettings.autoStereoEnabled ? "On" : "Off"}]`);
                console.log(`Image Stabilization: [${window.holographicSettings.imageStabilizationEnabled ? "On" : "Off"}]`);
                console.log(`World Origin Relative Position: [${window.holographicSettings.worldOriginRelativePosition}]`);
            }
            this.renderer = renderer;
            this._modelViewMatrix = [new three.Matrix4(), new three.Matrix4()];
            this._normalMatrix = [new three.Matrix3(), new three.Matrix3()];
            this._renderTargetArrayIndexBuffer = Float32Array.from([0.0, 1.0]);
            this._viewMatrixBuffer = new Float32Array(32);
            this._projectionMatrixBuffer = new Float32Array(32);
            this._modelViewMatrixBuffer = new Float32Array(32);
            this._normalMatrixBuffer = new Float32Array(18);
            this._cameraPositionBuffer = new Float32Array(6);
            this._ANGLE_instanced_arrays = this.renderer.extensions.get('ANGLE_instanced_arrays');
            this._shaderSourceOrig = renderer.context.shaderSource;
            this._linkProgramOrig = renderer.context.linkProgram;
            this._drawElementsInstancedANGLEOrig = this._ANGLE_instanced_arrays.drawElementsInstancedANGLE;
            this._drawArraysInstancedANGLEOrig = this._ANGLE_instanced_arrays.drawArraysInstancedANGLE;
            this._shaderSourcePatched = this._makePatchedShaderSource();
            this._linkProgramPatched = this._makePatchedLinkProgram();
        }
        _makePatchedLinkProgram() {
            let scope = this;
            let glLinkProgram = this.renderer.context.linkProgram.bind(this.renderer.context);
            return function (program) {
                scope.renderer.context.bindAttribLocation(program, 1, 'aRenderTargetArrayIndex');
                glLinkProgram(program);
            };
        }
        _makePatchedShaderSource() {
            let scope = this;
            let glShaderSource = this.renderer.context.shaderSource.bind(this.renderer.context);
            return function (shader, source) {
                if (source.indexOf('gl_Position') > 0) source = scope._patchVertexShader(source);
                else source = scope._patchFragmentShader(source);
                glShaderSource(shader, source);
            };
        }
        _makePatchedDrawElementsInstancedANGLE(object) {
            let scope = this;
            let glDrawElementsInstancedANGLE = this._ANGLE_instanced_arrays.drawElementsInstancedANGLE.bind(this._ANGLE_instanced_arrays);
            return function (mode, count, type, offset, primcount) {
                scope._uploadUniforms(object.material.program);
                glDrawElementsInstancedANGLE(mode, count, type, offset, primcount);
            };
        }
        _makePatchedDrawArraysInstancedANGLE(object) {
            let scope = this;
            let glDrawArraysInstancedANGLE = this._ANGLE_instanced_arrays.drawArraysInstancedANGLE.bind(this._ANGLE_instanced_arrays);
            return function (mode, first, count, primcount) {
                scope._uploadUniforms(object.material.program);
                glDrawArraysInstancedANGLE(mode, first, count, primcount);
            };
        }
        _patchVertexShader(shader) {
            shader = shader.replace(/uniform mat4 viewMatrix;/, 'varying mat4 viewMatrix;');
            shader = shader.replace(/uniform mat4 projectionMatrix;/, 'mat4 projectionMatrix;');
            shader = shader.replace(/uniform mat4 modelViewMatrix;/, 'mat4 modelViewMatrix;');
            shader = shader.replace(/uniform mat3 normalMatrix;/, 'mat3 normalMatrix;');
            shader = shader.replace(/uniform vec3 cameraPosition;/, 'varying vec3 cameraPosition;');
            shader = shader.replace(/void\s+main\s*\(\s*\)\s*{/, `
            attribute float aRenderTargetArrayIndex;
            uniform mat4 holographicViewMatrix[2];
            uniform mat4 holographicProjectionMatrix[2];
            uniform mat4 holographicModelViewMatrix[2];
            uniform mat3 holographicNormalMatrix[2];
            uniform vec3 holographicCameraPosition[2];
            varying float vRenderTargetArrayIndex;
            void main () {
                int index = int(aRenderTargetArrayIndex);
                viewMatrix = holographicViewMatrix[index];
                projectionMatrix = holographicProjectionMatrix[index];
                modelViewMatrix = holographicModelViewMatrix[index];
                normalMatrix = holographicNormalMatrix[index];
                cameraPosition = holographicCameraPosition[index];
                vRenderTargetArrayIndex = aRenderTargetArrayIndex;
        `);
            return shader;
        }
        _patchFragmentShader(shader) {
            shader = shader.replace(/uniform mat4 viewMatrix;/, 'varying mat4 viewMatrix;');
            shader = shader.replace(/uniform vec3 cameraPosition;/, 'varying vec3 cameraPosition;');
            shader = shader.replace(/void\s+main\s*\(\s*\)\s*{/, `
            varying float vRenderTargetArrayIndex;
            void main () {
                int index = int(vRenderTargetArrayIndex);
        `);
            return shader;
        }
        _patchRawShaderMaterial(material) {
            let vertShader = material.vertexShader, fragShader = material.fragmentShader;
            let vertHasViewMatrix = !!vertShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+viewMatrix\s*;/);
            let vertHasProjectionMatrix = !!vertShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+projectionMatrix\s*;/);
            let vertHasModelViewMatrix = !!vertShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+modelViewMatrix\s*;/);
            let vertHasNormalMatrix = !!vertShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat3\s+normalMatrix\s*;/);
            let vertHasCameraPosition = !!vertShader.match(/uniform\s*((?:high|medium|low)p)?\s+vec3\s+cameraPosition\s*;/);
            let fragHasViewMatrix = !!fragShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+viewMatrix\s*;/);
            let fragHasProjectionMatrix = !!fragShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+projectionMatrix\s*;/);
            let fragHasModelViewMatrix = !!fragShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+modelViewMatrix\s*;/);
            let fragHasNormalMatrix = !!fragShader.match(/uniform\s*((?:high|medium|low)p)?\s+mat3\s+normalMatrix\s*;/);
            let fragHasCameraPosition = !!fragShader.match(/uniform\s*((?:high|medium|low)p)?\s+vec3\s+cameraPosition\s*;/);
            vertShader = vertShader.replace(
                /uniform\s*((?:high|medium|low)p)?\s+mat4\s+viewMatrix\s*;/,
                `${fragHasViewMatrix ? 'varying ' : ''}$1 mat4 viewMatrix;`
            );
            vertShader = vertShader.replace(
                /uniform\s*((?:high|medium|low)p)?\s+mat4\s+projectionMatrix\s*;/,
                `${fragHasProjectionMatrix ? 'varying ' : ''}$1 mat4 projectionMatrix;`
            );
            vertShader = vertShader.replace(
                /uniform\s*((?:high|medium|low)p)?\s+mat4\s+modelViewMatrix\s*;/,
                `${fragHasModelViewMatrix ? 'varying ' : ''}$1 mat4 modelViewMatrix;`
            );
            vertShader = vertShader.replace(
                /uniform\s*((?:high|medium|low)p)?\s+mat3\s+normalMatrix\s*;/,
                `${fragHasNormalMatrix ? 'varying ' : ''}$1 mat3 normalMatrix;`
            );
            vertShader = vertShader.replace(
                /uniform\s*((?:high|medium|low)p)?\s+vec3\s+cameraPosition\s*;/,
                `${fragHasCameraPosition ? 'varying ' : ''} $1 vec3 cameraPosition;`
            );
            vertShader = vertShader.replace(/void\s+main\s*\(\s*\)\s*{/, `
            attribute float aRenderTargetArrayIndex;
            ${!vertHasViewMatrix && fragHasViewMatrix ? 'mat4 viewMatrix;' : ''}
            ${!vertHasProjectionMatrix && fragHasProjectionMatrix ? 'mat4 projectionMatrix;' : ''}
            ${!vertHasModelViewMatrix && fragHasModelViewMatrix ? 'mat4 modelViewMatrix;' : ''}
            ${!vertHasNormalMatrix && fragHasNormalMatrix ? 'mat3 normalMatrix;' : ''}
            ${!vertHasCameraPosition && fragHasCameraPosition ? 'vec3 cameraPosition;' : ''}
            ${vertHasViewMatrix || fragHasViewMatrix ? 'uniform mat4 holographicViewMatrix[2];' : ''}
            ${vertHasProjectionMatrix || fragHasProjectionMatrix ? 'uniform mat4 holographicProjectionMatrix[2];' : ''}
            ${vertHasModelViewMatrix || fragHasModelViewMatrix ? 'uniform mat4 holographicModelViewMatrix[2];' : ''}
            ${vertHasNormalMatrix || fragHasNormalMatrix ? 'uniform mat3 holographicNormalMatrix[2];' : ''}
            ${vertHasCameraPosition || fragHasCameraPosition ? 'uniform vec3 holographicCameraPosition[2];' : ''}
            varying float vRenderTargetArrayIndex;
            void main () {
                int index = int(aRenderTargetArrayIndex);
                ${vertHasViewMatrix || fragHasViewMatrix ? 'viewMatrix = holographicViewMatrix[index];' : ''}
                ${vertHasProjectionMatrix || fragHasProjectionMatrix ? 'projectionMatrix = holographicProjectionMatrix[index];' : ''}
                ${vertHasModelViewMatrix || fragHasModelViewMatrix ? 'modelViewMatrix = holographicModelViewMatrix[index];' : ''}
                ${vertHasNormalMatrix || fragHasNormalMatrix ? 'normalMatrix = holographicNormalMatrix[index];' : ''}
                ${vertHasCameraPosition || fragHasCameraPosition ? 'cameraPosition = holographicCameraPosition[index];' : ''}
                vRenderTargetArrayIndex = aRenderTargetArrayIndex;
        `);
            fragShader = fragShader.replace(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+viewMatrix\s*;/, 'varying $1 mat4 viewMatrix;');
            fragShader = fragShader.replace(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+projectionMatrix\s*;/, 'varying $1 mat4 projectionMatrix;');
            fragShader = fragShader.replace(/uniform\s*((?:high|medium|low)p)?\s+mat4\s+modelViewMatrix\s*;/, 'varying $1 mat4 modelViewMatrix;');
            fragShader = fragShader.replace(/uniform\s*((?:high|medium|low)p)?\s+mat3\s+normalMatrix\s*;/, 'varying $1 mat3 normalMatrix;');
            fragShader = fragShader.replace(/uniform\s*((?:high|medium|low)p)?\s+vec3\s+cameraPosition\s*;/, 'varying $1 vec3 cameraPosition;');
            fragShader = fragShader.replace(/void\s+main\s*\(\s*\)\s*{/, `
            varying highp float vRenderTargetArrayIndex;
            void main () {
                int index = int(vRenderTargetArrayIndex);
        `);
            material.vertexShader = vertShader;
            material.fragmentShader = fragShader;
        }
        _uploadUniforms(program) {
            let uniforms = program.getUniforms();
            if (uniforms.map['holographicViewMatrix']) uniforms.map['holographicViewMatrix'].setValue(this.renderer.context, this._viewMatrixBuffer);
            if (uniforms.map['holographicProjectionMatrix']) uniforms.map['holographicProjectionMatrix'].setValue(this.renderer.context, this._projectionMatrixBuffer);
            if (uniforms.map['holographicCameraPosition']) uniforms.map['holographicCameraPosition'].setValue(this.renderer.context, this._cameraPositionBuffer);
            if (uniforms.map['holographicModelViewMatrix']) uniforms.map['holographicModelViewMatrix'].setValue(this.renderer.context, this._modelViewMatrixBuffer);
            if (uniforms.map['holographicNormalMatrix']) uniforms.map['holographicNormalMatrix'].setValue(this.renderer.context, this._normalMatrixBuffer);
        }
        _updateAttributes(holographicAttributes, attributes) {
            for (var name in attributes) {
                let attribute = attributes[name];
                if (attribute.isInstancedBufferAttribute) {
                    let holographicAttribute = holographicAttributes[name];
                    if (!holographicAttribute) {
                        holographicAttribute = holographicAttributes[name] = new three.InstancedBufferAttribute(attribute.array, attribute.itemSize, attribute.meshPerAttribute * 2);
                        holographicAttribute.onUploadCallback = () => attribute.onUploadCallback();
                    }
                    else {
                        holographicAttribute.itemSize = attribute.itemSize;
                        holographicAttribute.meshPerAttribute = attribute.meshPerAttribute * 2;
                        if (holographicAttribute.array !== attribute.array) holographicAttribute.setArray(attribute.array);
                    }
                    holographicAttribute.normalized = attribute.normalized;
                    holographicAttribute.dynamic = attribute.dynamic;
                    holographicAttribute.updateRange = attribute.updateRange;
                    holographicAttribute.version = attribute.version;
                }
                else holographicAttributes[name] = attributes[name];
            }
            for (name in holographicAttributes) if (name !== 'aRenderTargetArrayIndex' && !attributes[name]) delete holographicAttributes[name];
        }
        _updateBufferGeometry(holographicGeometry, geometry) {
            holographicGeometry.groups = geometry.groups;
            holographicGeometry.index = geometry.index;
            holographicGeometry.maxInstancedCount = geometry.maxInstancedCount > 0 ? geometry.maxInstancedCount * 2 : 2;
            holographicGeometry.boundingBox = geometry.boundingBox;
            holographicGeometry.boundingSphere = geometry.boundingSphere;
            holographicGeometry.drawRange = geometry.drawRange;
            this._updateAttributes(holographicGeometry.attributes, geometry.attributes);
            this._updateAttributes(holographicGeometry.morphAttributes, geometry.morphAttributes);
        }
        _initHolographicMaterial(object) {
            let material = object.material;
            let holographicMaterial = object.material.__holographicMaterial;
            if (holographicMaterial === undefined) {
                holographicMaterial = object.material.__holographicMaterial = new material.constructor();
                holographicMaterial.addEventListener('dispose', holographicMaterial.__disposeCallback = event => this._onMaterialDispose(event));
                for (var name in material) holographicMaterial[name] = material[name];
                if (holographicMaterial.isRawShaderMaterial) this._patchRawShaderMaterial(holographicMaterial);
            }
            else {
                for (name in material) {
                    if (name === 'vertexShader' || name === 'fragmentShader') continue;
                    holographicMaterial[name] = material[name];
                }
            }
            return holographicMaterial;
        }
        _initHolographicGeometry(object) {
            let geometry = object.geometry;
            let holographicGeometry = object.geometry.__holographicGeometry;
            if (holographicGeometry === undefined) {
                holographicGeometry = object.geometry.__holographicGeometry = new three.InstancedBufferGeometry();
                holographicGeometry.addEventListener('dispose', holographicGeometry.__disposeCallback = event => this._onGeometryDispose(event));
                if (geometry.isBufferGeometry) this._updateBufferGeometry(holographicGeometry, geometry);
                else {
                    holographicGeometry.setFromObject(object);
                    holographicGeometry.maxInstancedCount = 2;
                }
                holographicGeometry.addAttribute('aRenderTargetArrayIndex', new three.InstancedBufferAttribute(this._renderTargetArrayIndexBuffer, 1, holographicGeometry.maxInstancedCount / 2));
            }
            else {
                if (geometry.isBufferGeometry) this._updateBufferGeometry(holographicGeometry, geometry);
                else holographicGeometry.updateFromObject(object);
            }
            return holographicGeometry;
        }
        _initHolographicCache(object) {
            return object.__holographicCache || {
                onBeforeRenderPatched: (...args) => this._onBeforeRender(object, ...args),
                onAfterRenderPatched: (...args) => this._onAfterRender(object, ...args),
                drawElementsInstancedANGLEPatched: this._makePatchedDrawElementsInstancedANGLE(object),
                drawArraysInstancedANGLEPatched: this._makePatchedDrawArraysInstancedANGLE(object)
            };
        }
        _onBeforeRender(object, renderer, scene, camera, geometry, material, group) {
            let cached = object.__holographicCache;
            cached.onBeforeRender(object, renderer, scene, camera, cached.geometry, material, group);
            cached.onAfterRender = object.onAfterRender;
            object.onAfterRender = cached.onAfterRenderPatched;
            this._modelViewMatrix[0].multiplyMatrices(camera.left ? camera.left.matrixWorldInverse : camera.matrixWorldInverse, object.matrixWorld);
            this._modelViewMatrix[1].multiplyMatrices(camera.right ? camera.right.matrixWorldInverse : camera.matrixWorldInverse, object.matrixWorld);
            this._normalMatrix[0].getNormalMatrix(this._modelViewMatrix[0]);
            this._normalMatrix[1].getNormalMatrix(this._modelViewMatrix[1]);
            this._modelViewMatrixBuffer.set(this._modelViewMatrix[0].elements, 0);
            this._modelViewMatrixBuffer.set(this._modelViewMatrix[1].elements, 16);
            this._normalMatrixBuffer.set(this._normalMatrix[0].elements, 0);
            this._normalMatrixBuffer.set(this._normalMatrix[1].elements, 9);
            this._viewMatrixBuffer.set(camera.left ? camera.left.matrixWorldInverse.elements : camera.matrixWorldInverse.elements, 0);
            this._viewMatrixBuffer.set(camera.right ? camera.right.matrixWorldInverse.elements : camera.matrixWorldInverse.elements, 16);
            this._projectionMatrixBuffer.set(camera.left ? camera.left.projectionMatrix.elements : camera.projectionMatrix.elements, 0);
            this._projectionMatrixBuffer.set(camera.right ? camera.right.projectionMatrix.elements : camera.projectionMatrix.elements, 16);
            this._cameraPositionBuffer.set(camera.left ? camera.left.position.toArray() : camera.position.toArray(), 0);
            this._cameraPositionBuffer.set(camera.right ? camera.right.position.toArray() : camera.position.toArray(), 3);
            if (!material.isRawShaderMaterial) this.renderer.context.shaderSource = this._shaderSourcePatched;
            this.renderer.context.linkProgram = this._linkProgramPatched;
            this._ANGLE_instanced_arrays.drawElementsInstancedANGLE = cached.drawElementsInstancedANGLEPatched;
            this._ANGLE_instanced_arrays.drawArraysInstancedANGLE = cached.drawArraysInstancedANGLEPatched;
        }
        _onAfterRender(object, renderer, scene, camera, geometry, material, group) {
            let cached = object.__holographicCache;
            this.renderer.context.shaderSource = this._shaderSourceOrig;
            this.renderer.context.linkProgram = this._linkProgramOrig;
            this._ANGLE_instanced_arrays.drawElementsInstancedANGLE = this._drawElementsInstancedANGLEOrig;
            this._ANGLE_instanced_arrays.drawArraysInstancedANGLE = this._drawElementsInstancedANGLEOrig;
            object.onAfterRender = cached.onAfterRender;
            object.onAfterRender(renderer, scene, camera, geometry, material, group);
        }
        _onMaterialDispose(event) {
            let material = event.target;
            material.__holographicMaterial.dispose();
            delete material.__holographicMaterial;
            material.removeEventListener('dispose', material.__holographicMaterial.__disposeCallback);
        }
        _onGeometryDispose(event) {
            let geometry = event.target;
            geometry.__holographicGeometry.dispose();
            delete geometry.__holographicGeometry;
            geometry.removeEventListener('dispose', geometry.__holographicGeometry.__disposeCallback);
        }
        _setupInstancing(object) {
            if (object.geometry === undefined || object.material === undefined) return;
            object.__holographicCache = this._initHolographicCache(object);
            object.__holographicCache.onBeforeRender = object.onBeforeRender;
            object.onBeforeRender = object.__holographicCache.onBeforeRenderPatched;
            object.__holographicCache.geometry = object.geometry;
            object.__holographicCache.material = object.material;
            object.geometry = this._initHolographicGeometry(object);
            object.material = this._initHolographicMaterial(object);
        }
        _teardownInstancing(object) {
            if (object.geometry === undefined || object.material === undefined) return;
            object.onBeforeRender = object.__holographicCache.onBeforeRender;
            object.geometry = object.__holographicCache.geometry;
            object.material = object.__holographicCache.material;
        }
        render(scene, camera, renderTarget, forceClear) {
            if (window.experimentalHolographic === true) {
                if (!camera.isHolographicCamera) throw new Error("Camera must be an instance of HolographicCamera");
                camera.update();
            }
            if (window.experimentalHolographic !== true || window.holographicSettings.autoStereoEnabled === true) {
                this.renderer.render(scene, camera, renderTarget, forceClear);
            }
            else {
                scene.traverse(o => this._setupInstancing(o));
                this.renderer.render(scene, camera, renderTarget, forceClear);
                scene.traverse(o => this._teardownInstancing(o));
            }
        }
    }

    class HolographicCamera extends three.Camera {
        constructor() {
            super();
            this.type = 'HolographicCamera';
            this.isHolographicCamera = true;
            this.left = new three.Camera();
            this.right = new three.Camera();
            this._cameraMap = [this.left, this, this.right];
            this._holographicViewMatrix = [new three.Matrix4(), new three.Matrix4(), new three.Matrix4()];
            this._holographicTransformMatrix = [new three.Matrix4(), new three.Matrix4(), new three.Matrix4()];
            this._holographicProjectionMatrix = [new three.Matrix4(), new three.Matrix4(), new three.Matrix4()];
        }
        update() {
            if (!window.experimentalHolographic) return;
            let params = window.getHolographicCameraParameters();
            this._holographicViewMatrix[0].fromArray(params.left.viewMatrix);
            this._holographicViewMatrix[1].fromArray(params.mid.viewMatrix);
            this._holographicViewMatrix[2].fromArray(params.right.viewMatrix);
            this._holographicProjectionMatrix[0].fromArray(params.left.projectionMatrix);
            this._holographicProjectionMatrix[1].fromArray(params.mid.projectionMatrix);
            this._holographicProjectionMatrix[2].fromArray(params.right.projectionMatrix);
            this._holographicViewMatrix.forEach((viewMatrix, i) => {
                this._holographicTransformMatrix[i].getInverse(viewMatrix);
                this._holographicTransformMatrix[i].decompose(this._cameraMap[i].position, this._cameraMap[i].quaternion, this._cameraMap[i].scale);
            });
            this._holographicProjectionMatrix.forEach((projectionMatrix, i) => {
                this._cameraMap[i].projectionMatrix.copy(projectionMatrix);
            });
            if (this.left.parent === null) {
                this.left.updateMatrixWorld();
                this.left.matrixWorldInverse.copy(this._holographicViewMatrix[0]);
            }
            if (this.right.parent === null) {
                this.right.updateMatrixWorld();
                this.right.matrixWorldInverse.copy(this._holographicViewMatrix[2]);
            }
        }
    }

    exports.HolographicEffect = HolographicEffect;
    exports.HolographicCamera = HolographicCamera;

    Object.defineProperty(exports, '__esModule', { value: true });

})));
