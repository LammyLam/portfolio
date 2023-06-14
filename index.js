let siuMeiIndex = 1;
let jsyncIndex = 1;
let siuMeiSlides;
let jSyncSlides;

function setSlide() {
	[...siuMeiSlides].forEach((slide, index) => {
		if (siuMeiIndex === index) {
			slide.classList.add("show");
		} else {
			slide.classList.remove("show");
		}
	});
	siuMeiIndex = (siuMeiIndex + 1) % siuMeiSlides.length;

	[...jSyncSlides].forEach((slide, index) => {
		if (jsyncIndex === index) {
			slide.classList.add("show");
		} else {
			slide.classList.remove("show");
		}
	});
	jsyncIndex = (jsyncIndex + 1) % jSyncSlides.length;
}

function onMouseMove(e) {
	console.log(e.screenX, e.screenY);
}

function init() {
	siuMeiSlides = document.querySelectorAll("img.project-img-slide-siumei");
	jSyncSlides = document.querySelectorAll("img.project-img-slide-jsync");
	setInterval(setSlide, 3000);

	const observer = new IntersectionObserver((entries) => {
		entries.forEach((entry) => {
			if (entry.isIntersecting) {
				entry.target.classList.add("anim-show");
				observer.unobserve(entry.target);
			}
		});
	});

	const animElements = document.querySelectorAll(
		".anim-left-2-right, .anim-right-2-left, .anim-up-2-down, .icon-contact, .anim-fade"
	);
	animElements.forEach((el) => observer.observe(el));
}

window.onload = init;
