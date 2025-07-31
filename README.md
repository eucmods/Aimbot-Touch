# Aimbot-Touch

 * 🔫 AIMBOTTOUCH.h – Sistema de Aimbot com Simulação de Toque
 *
 * Descrição:
 * Esta biblioteca implementa um sistema de Aimbot voltado para jogos Android,
 * utilizando simulação direta de toques na tela via /dev/input/event.
 * Ao identificar a posição da cabeça do inimigo, o código calcula as coordenadas
 * ideais para realizar um "subir capa" automático (headshot) e simula toques 
 * precisos na tela com base nesses dados.
 *
 * O toque é enviado diretamente ao dispositivo de entrada (touchscreen),
 * utilizando eventos do tipo EV_ABS e EV_KEY, com sincronização por EV_SYN.
 * 
 * Principais funções:
 * - Touch_Init()    -> Inicializa o acesso ao /dev/input/eventX correto.
 * - Touch_Down(x,y) -> Simula o início do toque (dedo pressionado).
 * - Touch_Move(x,y) -> Simula movimentação do dedo (opcional).
 * - Touch_Up()      -> Simula o fim do toque (dedo levantado).
 * 
 * Requisitos:
 * - Dispositivo Android com acesso root.
 * - Permissões adequadas para escrita em /dev/input/eventX.
 * - Identificação correta do event correspondente ao touchscreen (ex: /dev/input/event2).
 *
 * Possíveis Erros e Incompatibilidades:
 * - "Permission denied": dispositivo sem root ou proteção SELinux ativa.
 * - "No such file or directory": eventX incorreto ou não encontrado.
 * - Toque não funcionando: pode estar escrevendo no event errado (não é o touchscreen).
 * - Dispositivos que não possuem /dev/uinput ativado podem precisar de outro método.
 *
 * ⚠️ Observação:
 * Em alguns aparelhos, por segurança do sistema, o kernel pode bloquear o envio de eventos simulados,
 * mesmo com root. Nestes casos, o AIMBOTTOUCH não funcionará a menos que o kernel seja modificado
 * ou um módulo externo seja usado.
 *
 * Desenvolvido por: CMODs


