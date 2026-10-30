// Physical keyboard support. All game logic stays on the server; this just
// forwards key presses to POST /key through htmx. Requests are chained so
// fast typing is never dropped by htmx's default "queue last" behaviour.
(() => {
  let chain = Promise.resolve();

  function send(k) {
    chain = chain
      .then(() => htmx.ajax("POST", "press/" + encodeURIComponent(k), {
        swap: "none",
      }))
      .catch(() => { });
  }

  document.addEventListener("keydown", (e) => {
    if (e.ctrlKey || e.metaKey || e.altKey) return;

    let k = null;
    if (/^[a-zA-Z]$/.test(e.key)) k = e.key.toLowerCase();
    else if (e.key === "Enter") k = "enter";
    else if (e.key === "Backspace") k = "back";
    if (!k) return;

    // Stops a focused button from also firing its own click on Enter.
    e.preventDefault();
    send(k);
  });
})();
