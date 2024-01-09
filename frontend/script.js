window.addEventListener('scroll', function() {
   
    var navbar = document.getElementById('navbar');

    if (window.scrollY > 50) {
      navbar.classList.add('white-bg');
    } else {
      navbar.classList.remove('white-bg');
    }
  });