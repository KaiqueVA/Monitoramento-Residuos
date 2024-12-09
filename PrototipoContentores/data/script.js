const form = document.getElementById('lorawan-form');

form.addEventListener('submit', async (e) => {
  e.preventDefault();

  // Obter valores do formulário
  const devaddr = document.getElementById('devaddr').value;
  const nwkskey = document.getElementById('nwkskey').value;
  const appskey = document.getElementById('appskey').value;

  // Enviar dados ao servidor
  const response = await fetch('/config-lorawan', {
    method: 'POST',
    headers: {
      'Content-Type': 'application/json',
    },
    body: JSON.stringify({ devaddr, nwkskey, appskey }),
  });

  if (response.ok) {
    alert('Configuração enviada com sucesso!');
  } else {
    alert('Erro ao enviar configuração!');
  }
});
