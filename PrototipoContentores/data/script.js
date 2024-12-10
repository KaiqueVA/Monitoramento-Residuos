document.getElementById("config-form").addEventListener("submit", async (event) => {
  event.preventDefault();

  const devaddr = document.getElementById("devaddr").value.trim();
  const nwkskey = document.getElementById("nwkskey").value.trim();
  const appskey = document.getElementById("appskey").value.trim();

  const payload = {
    devaddr: devaddr,
    nwkskey: nwkskey,
    appskey: appskey,
  };

  try {
    const response = await fetch("/config-lorawan", {
      method: "POST",
      headers: {
        "Content-Type": "application/json",
      },
      body: JSON.stringify(payload),
    });

    const result = await response.json();
    if (result.status === "success") {
      alert("Configuração salva com sucesso!");
    } else {
      alert(`Erro: ${result.message}`);
    }
  } catch (error) {
    alert("Erro ao enviar configuração: " + error.message);
  }
});
