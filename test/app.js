const msg = document.getElementById("message");
const btn = document.getElementById("btn");

btn.addEventListener("click", () => {
    const now = new Date();
    msg.textContent = "Clicked at: " + now.toLocaleTimeString();
});
