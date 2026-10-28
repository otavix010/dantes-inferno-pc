# Novidades do port

O Otavix Launcher mostra este arquivo na página "Novidades" do jogo. Cada atualização tem um título
"## <data> · <versão>" e o texto em português (### pt), inglês (### en) e, quando houver, espanhol (### es).
As mais novas ficam em cima.

## 27/10/2026 · deaed34
### pt
- **Pasta no lugar da .iso:** escolher a pasta onde a .iso está (mesmo numa subpasta) agora funciona; o gerador acha a .iso sozinho. Se a pasta não tiver uma .iso (por exemplo, o jogo já extraído ou no formato Games on Demand), aparece uma mensagem explicando o que escolher, em vez de "acesso negado".
### en
- **A folder instead of the .iso:** picking the folder the .iso is in (even in a subfolder) now works; the generator finds the .iso by itself. If the folder has no .iso (an unpacked game or a Games on Demand folder, for example), a message says what to pick, instead of "access denied".
### es
- **Carpeta en lugar de la .iso:** elegir la carpeta donde está la .iso (incluso en una subcarpeta) ahora funciona; el generador encuentra la .iso solo. Si la carpeta no tiene una .iso (el juego ya extraído o en formato Games on Demand, por ejemplo), aparece un mensaje que explica qué elegir, en lugar de "acceso denegado".

## 26/10/2026 · 9c6094e
### pt
- **Estátua de Beatriz:** o jogo fechava com erro (0xC0000005) ao usar a primeira estátua para salvar. A tela de salvar mostra uma miniatura do save, e o tratamento de erro do leitor dessa imagem não voltava ao ponto certo no código traduzido. Agora salvar, sobrescrever e carregar funcionam.
- **Relatório de erro mais completo:** quando o jogo fecha por erro, o arquivo de falha que vai junto no report registra também os fechamentos do tipo 0xC0000409, com o ponto exato do problema.
### en
- **Beatrice statue:** the game closed with an error (0xC0000005) when using the first statue to save. The save screen shows a thumbnail of the save, and the error handling of that image's reader did not return to the right place in the translated code. Saving, overwriting and loading now work.
- **More complete error report:** when the game closes with an error, the crash file sent with the report now also records 0xC0000409 closes, with the exact point of the problem.
