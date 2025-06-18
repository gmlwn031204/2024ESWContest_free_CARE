import 'package:flutter/material.dart';
import 'Main_page.dart';

class Message_page extends StatelessWidget {
  final String name;
  final String relation;
  final String phone;

  const Message_page({
    super.key,
    required this.name,
    required this.relation,
    required this.phone,
  });

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      backgroundColor: Colors.white,
      body: Center(
        child: Padding(
          padding: const EdgeInsets.symmetric(horizontal: 40.0),
          child: Column(
            mainAxisAlignment: MainAxisAlignment.start,
            crossAxisAlignment: CrossAxisAlignment.center,
            children: [
              const SizedBox(height: 145),
              // SafeRide 로고
              Text(
                'SafeRide',
                style: TextStyle(
                  fontSize: 73,
                  fontWeight: FontWeight.bold,
                  letterSpacing: 1.5,
                  color: Colors.black,
                  shadows: [
                    Shadow(
                      blurRadius: 8.0,
                      color: Colors.black26,
                      offset: Offset(0, 4),
                    ),
                  ],
                ),
              ),
              const SizedBox(height: 15),
              // 아이콘 대신 이미지
              SizedBox(
                height: 220,
                child: Image.asset(
                  'assets/message.png',
                  width: 180,
                  height: 180,
                  fit: BoxFit.contain,
                ),
              ),
              const SizedBox(height: 15),
              // 안내문구
              Container(
                width: double.infinity,
                padding: const EdgeInsets.symmetric(vertical: 16, horizontal: 12),
                decoration: BoxDecoration(
                  color: Colors.red[50],
                  border: Border.all(color: Colors.red, width: 2),
                  borderRadius: BorderRadius.circular(10),
                ),
                child: const Text(
                  '1분 동안 해제가 감지되지 않아\n보호자에게 사고 알림 문자가\n발송되었습니다.\n상황이 안정되셨다면 앱을 통해\n복귀해 주시기 바랍니다.',
                  style: TextStyle(
                    color: Colors.red,
                    fontSize: 18,
                    fontWeight: FontWeight.bold,
                  ),
                  textAlign: TextAlign.center,
                ),
              ),
              const SizedBox(height: 35),
              SizedBox(
                width: double.infinity,
                height: 50,
                child: ElevatedButton(
                  onPressed: () {
                    Navigator.pushAndRemoveUntil(
                      context,
                      MaterialPageRoute(
                        builder: (context) => Main_page(
                          name: name,
                          relation: relation,
                          phone: phone,
                        ),
                      ),
                      (route) => false,  // 모든 이전 페이지 스택 제거
                    );
                  },
                  style: ElevatedButton.styleFrom(
                    backgroundColor: Colors.blue,
                    shape: RoundedRectangleBorder(
                      borderRadius: BorderRadius.circular(10),
                    ),
                  ),
                  child: const Text(
                    '홈으로 돌아가기',
                    style: TextStyle(
                      color: Colors.white,
                      fontSize: 18,
                      fontWeight: FontWeight.bold,
                    ),
                  ),
                ),
              ),
            ],
          ),
        ),
      ),
    );
  }
}
