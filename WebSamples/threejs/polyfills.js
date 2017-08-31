performance = {
    now: (() => {
        let start = Date.now();
        return () => Date.now() - start;
    })()
};

console.warn = (...x) => console.log(...x);

console.error = (...x) => console.log(...x);

console.info = (...x) => console.log(...x);

console.dir = (...x) => console.log(...x);
