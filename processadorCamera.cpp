/* Nome: Andrei Pochmann Koenich	Matricula: 00308680 */

/* O programa a seguir acessa a camera do usuario conectada ao computador, e permite que sejam realizadas
algumas operacoes de processamento de imagens em tempo real, sobre o video capturado pela camera. */

#define CVUI_IMPLEMENTATION

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/core/utils/logger.hpp>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <stdbool.h>
#include "cvui.h"

#define NOMEINTERFACE "USER INTERFACE" /* Constantes com os nomes das janelas. */
#define NOMEJANELACAMERAIN "INPUT VIDEO"
#define NOMEJANELACAMERAOUT "OUTPUT VIDEO"
#define NOMEVIDEOSAIDA "videosaida.avi"

#define X_CAMERAIN 400  /* Constantes com as coordenadas das janelas. */
#define Y_CAMERAIN 15
#define X_CAMERAOUT 400
#define Y_CAMERAOUT 500

#define DIMENSAORGB 3
#define ESC 27
#define R 2
#define G 1
#define B 0

#define LARGURABOTAO 150		/* Constantes dos elementos da interface. */
#define ALTURABOTAO 30
#define LARGURAINTERFACE 400
#define ALTURAINTERFACE 455

#define PULO_CONTRASTE 0.1
#define PULO_BRILHO 0.5
#define LIMITETRACKBAR 20
#define CODIGOFLIPHORIZONTAL 1
#define CODIGOFLIPVERTICAL 0

#define X_INTERFACE 1100	/* Constantes das coordenadas das janelas. */
#define Y_INTERFACE 100

#define TOTALCOMANDOS 13 /* Constantes referentes aos indices do vetor de comandos. */
#define OP_LUMINANCIA 0
#define OP_GAUSSIANBLUR 1
#define OP_FLIPHORIZONTAL 2
#define OP_FLIPVERTICAL 3
#define OP_ROTACIONAHORARIO 4
#define OP_ROTACIONAANTIHORARIO 5
#define OP_NEGATIVO 6
#define OP_CANNY 7
#define OP_SOBEL 8
#define OP_ZOOMOUT 9
#define OP_CONTRASTE 10
#define OP_BRILHO 11
#define OP_VIDEO 12

using namespace cv;
using namespace std;

void reiniciaVetor(int comandos[]) /* Reinicia todas as posicoes do vetor para zero. */
{
	for (int i = 0; i < TOTALCOMANDOS-1; i++) /* Reinicia todas as operacoes, com excecao da operacao de gravacao de video. */
		comandos[i] = 0;
}

void iniciaPrograma()
{
	Mat frame_interface = Mat(ALTURAINTERFACE, LARGURAINTERFACE, CV_8UC3); /* Declaracao do frame que corresponde a janela de interface. */

	namedWindow(NOMEINTERFACE);
	moveWindow(NOMEINTERFACE, X_INTERFACE, Y_INTERFACE); /* Posiciona a janela da interface em local adequado na tela. */
	cvui::init(NOMEINTERFACE);

	int cameraIn = 0;

	VideoCapture cap(0, CAP_V4L2); /* Usamos V4L2 no Linux. */

	if (!cap.open(cameraIn)) /* Abre a camera padrao conectada. */
		return;

	int frame_largura = cap.get(cv::CAP_PROP_FRAME_WIDTH);
	int frame_altura = cap.get(cv::CAP_PROP_FRAME_HEIGHT);
	VideoWriter video(NOMEVIDEOSAIDA, cv::VideoWriter::fourcc('M', 'J', 'P', 'G'), 10, Size(frame_largura, frame_altura));

	double contrast = 1;
	double brightness = 1;

	int comandos[TOTALCOMANDOS] = { 0 }; /* Vetor destinado a conter o numero de vezes que cada um dos 12 comandos foi executado. */
	int tamanho_kernel = 3;

	Mat inCamera; /* Variaveis para conter o frame do video detectado pela camera. */
	Mat outCamera;
	Mat auxCamera;

	while (true) /* Iteração para processamento de cada frame capturado pela camera. */
	{
		cap >> inCamera;
		cap >> outCamera;

		if (inCamera.empty())
			break; /* Fim do streaming da camera. */

		if (outCamera.empty())
			break; /* Fim do streaming da camera. */

		frame_interface = cv::Scalar(49, 52, 49); /* Determina as cores da interface. */

		cvui::text(frame_interface, 265, 200, "Contrast");					/* Interface para mudanca do valor de ganho a ser utilizado na operacao de mudanca de contraste. */
		cvui::counter(frame_interface, 245, 215, &contrast, PULO_CONTRASTE);

		cvui::text(frame_interface, 260, 295, "Brightness");					/* Interface para mudanca do valor de bias a ser utilizado na operacao de mudanca de brilho. */
		cvui::counter(frame_interface, 245, 310, &brightness, PULO_BRILHO);

		if (cvui::button(frame_interface, 40, 35, LARGURABOTAO + 175, ALTURABOTAO, "Reset")) /* Realiza a copia do conteudo original da camera. */
		{
			reiniciaVetor(comandos); /* Reinicia todas as posicoes do vetor para zero. */
			destroyWindow(NOMEJANELACAMERAOUT); /* Fecha a janela contendo o video antigo. */
			imshow(NOMEJANELACAMERAOUT, outCamera); /* Mostra novamente o video reiniciado. */
		}

		if (cvui::button(frame_interface, 40, 75, LARGURABOTAO, ALTURABOTAO, "Luminance")) /* Aplica luminancia no video. */
			if (comandos[OP_LUMINANCIA] == 0) /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */
				comandos[OP_LUMINANCIA] = 1;

		if (comandos[OP_LUMINANCIA] != 0) /* Verifica se o vetor de comandos contem a operacao de luminancia escolhida. */
		{
			auxCamera = outCamera.clone();
			cvtColor(auxCamera, outCamera, COLOR_BGR2GRAY);
			cvtColor(outCamera, outCamera, COLOR_GRAY2BGR);
		}

		if (cvui::button(frame_interface, 40, 115, LARGURABOTAO, ALTURABOTAO, "Gaussian Blur")) /* Aplica borradura com o filtro Gaussiano. */
		{
			createTrackbar("KernelSize", NOMEJANELACAMERAOUT, &tamanho_kernel, LIMITETRACKBAR);

			if (comandos[OP_GAUSSIANBLUR] == 0) /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */
				comandos[OP_GAUSSIANBLUR] = 1;
		}

		if (comandos[OP_GAUSSIANBLUR] != 0) /* Verifica se o vetor de comandos contem a operacao de Gaussian Blur escolhida. */
			GaussianBlur(outCamera, outCamera, Size(2 * tamanho_kernel + 1, 2 * tamanho_kernel + 1), 3, 0); /* Aplica convolucao para gerar borradura, garantindo que o kernel sempre possua dimensoes impares. */

		if (cvui::button(frame_interface, 40, 155, LARGURABOTAO, ALTURABOTAO, "Horizontal Flip")) /* Aplica espelhamento horizontal. */
		{
			if (comandos[OP_FLIPHORIZONTAL] == 0)
				comandos[OP_FLIPHORIZONTAL]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

			else if (comandos[OP_FLIPHORIZONTAL] == 1)
				comandos[OP_FLIPHORIZONTAL] = 0;
		}

		if (comandos[OP_FLIPHORIZONTAL] == 1) /* Verifica se o vetor de comandos contem a operacao de espelhamento horizontal escolhida. */
			flip(outCamera, outCamera, CODIGOFLIPHORIZONTAL); /* Realiza o flip horizontal. */

		if (cvui::button(frame_interface, 40, 195, LARGURABOTAO, ALTURABOTAO, "Vertical Flip")) /* Aplica espelhamento vertical. */
		{
			if (comandos[OP_FLIPVERTICAL] == 0)
				comandos[OP_FLIPVERTICAL]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

			else if (comandos[OP_FLIPVERTICAL] == 1)
				comandos[OP_FLIPVERTICAL] = 0;
		}

		if (comandos[OP_FLIPVERTICAL] == 1) /* Verifica se o vetor de comandos contem a operacao de espelhamento vertical escolhida. */
			flip(outCamera, outCamera, CODIGOFLIPVERTICAL); /* Realiza o flip vertical. */

		if (cvui::button(frame_interface, 40, 235, LARGURABOTAO, ALTURABOTAO, "Rotate +90"))/* Rotaciona o video em +90 graus. */
		{
			comandos[OP_ROTACIONAHORARIO]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

			if (comandos[OP_ROTACIONAHORARIO] == 4)
				comandos[OP_ROTACIONAHORARIO] = 0;
		}

		if (comandos[OP_ROTACIONAHORARIO] != 0) /* Verifica se o vetor de comandos contem a operacao de rotacao em +90 graus escolhida. */
			for (int i = 0; i < comandos[OP_ROTACIONAHORARIO]; i++)
				cv::rotate(outCamera, outCamera, cv::ROTATE_90_COUNTERCLOCKWISE);

		if (cvui::button(frame_interface, 40, 275, LARGURABOTAO, ALTURABOTAO, "Rotate -90")) /* Rotaciona o video em -90 graus. */
		{
			comandos[OP_ROTACIONAANTIHORARIO]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

			if (comandos[OP_ROTACIONAANTIHORARIO] == 4)
				comandos[OP_ROTACIONAANTIHORARIO] = 0;
		}

		if (comandos[OP_ROTACIONAANTIHORARIO] != 0)  /* Verifica se o vetor de comandos contem a operacao de rotacao em +90 graus escolhida. */
			for (int i = 0; i < comandos[OP_ROTACIONAANTIHORARIO]; i++)
				cv::rotate(outCamera, outCamera, cv::ROTATE_90_CLOCKWISE);

		if (cvui::button(frame_interface, 40, 315, LARGURABOTAO, ALTURABOTAO, "Negative")) /* Calcula o negativo do video. */
		{
			comandos[OP_NEGATIVO]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

			if (comandos[OP_NEGATIVO] == 2) /* Verifica se a operacao foi aplicada um numero par de vezes, para zerar o contador. */
				comandos[OP_NEGATIVO] = 0;
		}

		if (comandos[OP_NEGATIVO] != 0) /* Verifica se o vetor de comandos contem a operacao de negativo escolhida. */
			outCamera.convertTo(outCamera, -1, -1, 255);

		if (cvui::button(frame_interface, 215, 75, LARGURABOTAO, ALTURABOTAO, "Canny")) /* Utiliza o comando Canny para detectar arestas no video. */
			comandos[OP_CANNY]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

		if (comandos[OP_CANNY] != 0) /* Verifica se o vetor de comandos contem a operacao de Canny escolhida. */
		{
			for (int i = 0; i < comandos[OP_CANNY]; i++) /* Realiza a operacao de Canny o numero de vezes necessario. */
			{
				auxCamera = outCamera.clone(); /* Armazena o conteudo do frame em uma variavel auxiliar. */
				Canny(auxCamera, outCamera, 50, 100);
				cvtColor(outCamera, outCamera, COLOR_GRAY2BGR);
			}
		}

		if (cvui::button(frame_interface, 215, 115, LARGURABOTAO, ALTURABOTAO, "Sobel")) /* Utiliza o comando Sobel para obter uma estimativa do gradiente do video. */
			comandos[OP_SOBEL]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

		if (comandos[OP_SOBEL] != 0) /* Verifica se o vetor de comandos contem a operacao de Sobel escolhida. */
		{
			for (int i = 0; i < comandos[OP_SOBEL]; i++) /* Realiza a operacao de Sobel o numero de vezes necessario. */
			{
				auxCamera = outCamera.clone(); /* Armazena o conteudo do frame em uma variavel auxiliar. */
				Sobel(auxCamera, outCamera, CV_8UC1, 0, 1, 3, 1, 1, BORDER_DEFAULT); /* Realiza a operacao de convolucao. */
			}
		}

		if (cvui::button(frame_interface, 215, 155, LARGURABOTAO, ALTURABOTAO, "Zoom Out 2x2")) /* Diminui as dimensoes do video pela metade.  */
			comandos[OP_ZOOMOUT]++; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

		if (comandos[OP_ZOOMOUT] != 0) /* Verifica se o vetor de comandos contem a operacao de Zoom Out escolhida. */
		{
			for (int i = 0; i < comandos[OP_ZOOMOUT]; i++)
			{
				auxCamera = outCamera.clone(); /* Armazena o conteudo do frame em uma variavel auxiliar. */
				resize(auxCamera, outCamera, Size((int)auxCamera.cols / 2, (int)auxCamera.rows / 2)); /* Altera o tamanho do frame. */
			}
		}

		if (cvui::button(frame_interface, 215, 250, LARGURABOTAO, ALTURABOTAO, "Change Contrast")) /* Altera o contraste do video. */
			comandos[OP_CONTRASTE] = 1; /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */

		if (comandos[OP_CONTRASTE] != 0) /* Verifica se o vetor de comandos contem a operacao de mudanca de contraste escolhida. */
			outCamera.convertTo(outCamera, -1, contrast, 0); /* Faz a mudanca de contraste. */

		if (cvui::button(frame_interface, 215, 345, LARGURABOTAO, ALTURABOTAO, "Change Brightness")) /* Altera o brilho do video. */
			comandos[OP_BRILHO] = 1;

		if (comandos[OP_BRILHO] != 0) /* Verifica se o vetor de comandos contem a operacao de mudanca de brilho escolhida. */
			outCamera.convertTo(outCamera, -1, 1, brightness); /* Faz a mudanca de brilho. */

		if (cvui::button(frame_interface, 40, 355, LARGURABOTAO, ALTURABOTAO, "Start/Stop Recording")) /* Cria um arquivo de video com alguns frames. */
		{
			if (comandos[OP_VIDEO] == 0) /* Atualiza o vetor de comandos, caso o comando tenha sido escolhido. */
				comandos[OP_VIDEO] = 1;
			
			else if (comandos[OP_VIDEO] == 1) /* Encerra a gravacao do video, caso o usuario pressione a tecla novamente.  */
			{
				comandos[OP_VIDEO] = 0;
				video.release();
			}
		}

		if (comandos[OP_VIDEO] == 1) /* Realiza a gravacao do frame em um video, caso o botao tenha sido pressionado. */
			video.write(outCamera);

		if (cvui::button(frame_interface, 40, 395, LARGURABOTAO + 175, ALTURABOTAO, "Quit")) /* Encerra a execucao do programa. */
			break;

		namedWindow(NOMEJANELACAMERAIN, WINDOW_AUTOSIZE);
		moveWindow(NOMEJANELACAMERAIN, X_CAMERAIN, Y_CAMERAIN); /* Posiciona a janela da interface em local adequado na tela. */
		imshow(NOMEJANELACAMERAIN, inCamera); /* Mostra o frame da iteracao da vez. */

		namedWindow(NOMEJANELACAMERAOUT, WINDOW_AUTOSIZE); 
		moveWindow(NOMEJANELACAMERAOUT, X_CAMERAOUT, Y_CAMERAOUT); /* Posiciona a janela da interface em local adequado na tela. */
		imshow(NOMEJANELACAMERAOUT, outCamera); /* Mostra o frame da iteracao da vez, apos alguma modificacao pelas funcoes. */

		cvui::update(NOMEINTERFACE);
		cvui::imshow(NOMEINTERFACE, frame_interface);

		if (waitKey(1) == ESC)
			break; /* Encerra a captura da camera quando o usuario pressiona ESC. */
	}

	video.release(); /* Libera o objeto video, apos o fim da iteracao. */
	cap.release(); /* Libera o objeto VideoCapture, apos o fim da iteracao. */
}

int main()
{
	utils::logging::setLogLevel(utils::logging::LOG_LEVEL_SILENT); /* Desabilita warnings desnecessarias do OpenCV. */
	iniciaPrograma();
	return 0;
}
